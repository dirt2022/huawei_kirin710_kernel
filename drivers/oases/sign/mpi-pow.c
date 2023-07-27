/* mpi-pow.c  -  MPI functions
 *	Copyright (C) 1994, 1996, 1998, 2000 Free Software Foundation, Inc.
 *
 * This file is part of GnuPG.
 *
 * GnuPG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GnuPG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * Note: This code is heavily based on the GNU MP Library.
 *	 Actually it's the same code with only minor changes in the
 *	 way the data is stored; this is to support the abstraction
 *	 of an optional secure memory allocation which may be used
 *	 to avoid revealing of sensitive data due to paging etc.
 *	 The GNU MP Library itself is published under the LGPL;
 *	 however I decided to publish this code under the plain GPL.
 */

#include "mpi-internal.h"
#include "longlong.h"
#include "mpi-inline.h"

static inline mpi_limb_t mpihelp_sub_1(mpi_ptr_t res_ptr, mpi_ptr_t s1_ptr,
                         mpi_size_t s1_size, mpi_limb_t s2_limb);
mpi_limb_t mpihelp_sub_n(mpi_ptr_t res_ptr, mpi_ptr_t s1_ptr,
                         mpi_ptr_t s2_ptr, mpi_size_t size);
static inline mpi_limb_t mpihelp_sub(mpi_ptr_t res_ptr, mpi_ptr_t s1_ptr, mpi_size_t s1_size,
                       mpi_ptr_t s2_ptr, mpi_size_t s2_size);

#ifndef oasses_mpihelp_sub
static inline mpi_limb_t oases_mpihelp_sub(mpi_ptr_t res_ptr, mpi_ptr_t s1_ptr, mpi_size_t s1_size,
            mpi_ptr_t s2_ptr, mpi_size_t s2_size);
#endif

/****************
 * RES = BASE ^ EXP mod MOD
 */
int oases_mpi_powm(MPI res, MPI base, MPI exp, MPI mod)
{
	mpi_ptr_t mp_marker = NULL, bp_marker = NULL, ep_marker = NULL;
	mpi_ptr_t xp_marker = NULL;
	mpi_ptr_t tspace = NULL;
	mpi_ptr_t rp, ep, mp, bp;
	mpi_size_t esize, msize, bsize, rsize;
	int esign, msign, bsign, rsign;
	mpi_size_t size;
	int mod_shift_cnt;
	int negative_result;
	int assign_rp = 0;
	mpi_size_t tsize = 0;	/* to avoid compiler warning */
	/* fixme: we should check that the warning is void */
	int rc = -ENOMEM;

	esize = exp->nlimbs;
	msize = mod->nlimbs;
	size = 2 * msize;
	esign = exp->sign;
	msign = mod->sign;

	rp = res->d;
	ep = exp->d;

	if (!msize)
		return -EINVAL;

	if (!esize) {
		/* Exponent is zero, result is 1 mod MOD, i.e., 1 or 0
		 * depending on if MOD equals 1.  */
		rp[0] = 1;
		res->nlimbs = (msize == 1 && mod->d[0] == 1) ? 0 : 1;
		res->sign = 0;
		goto leave;
	}

	/* Normalize MOD (i.e. make its most significant bit set) as required by
	 * mpn_divrem.  This will make the intermediate values in the calculation
	 * slightly larger, but the correct result is obtained after a final
	 * reduction using the original MOD value.  */
	mp = mp_marker = oases_mpi_alloc_limb_space(msize);
	if (!mp)
		goto enomem;
	mod_shift_cnt = count_leading_zeros(mod->d[msize - 1]);
	if (mod_shift_cnt)
		oases_mpihelp_lshift(mp, mod->d, msize, mod_shift_cnt);
	else
		MPN_COPY(mp, mod->d, msize);

	bsize = base->nlimbs;
	bsign = base->sign;
	if (bsize > msize) {	/* The base is larger than the module. Reduce it. */
		/* Allocate (BSIZE + 1) with space for remainder and quotient.
		 * (The quotient is (bsize - msize + 1) limbs.)  */
		bp = bp_marker = oases_mpi_alloc_limb_space(bsize + 1);
		if (!bp)
			goto enomem;
		MPN_COPY(bp, base->d, bsize);
		/* We don't care about the quotient, store it above the remainder,
		 * at BP + MSIZE.  */
		oases_mpihelp_divrem(bp + msize, 0, bp, bsize, mp, msize);
		bsize = msize;
		/* Canonicalize the base, since we are going to multiply with it
		 * quite a few times.  */
		MPN_NORMALIZE(bp, bsize);
	} else
		bp = base->d;

	if (!bsize) {
		res->nlimbs = 0;
		res->sign = 0;
		goto leave;
	}

	if (res->alloced < size) {
		/* We have to allocate more space for RES.  If any of the input
		 * parameters are identical to RES, defer deallocation of the old
		 * space.  */
		if (rp == ep || rp == mp || rp == bp) {
			rp = oases_mpi_alloc_limb_space(size);
			if (!rp)
				goto enomem;
			assign_rp = 1;
		} else {
			if (oases_mpi_resize(res, size) < 0)
				goto enomem;
			rp = res->d;
		}
	} else {		/* Make BASE, EXP and MOD not overlap with RES.  */
		if (rp == bp) {
			/* RES and BASE are identical.  Allocate temp. space for BASE.  */
			BUG_ON(bp_marker);
			bp = bp_marker = oases_mpi_alloc_limb_space(bsize);
			if (!bp)
				goto enomem;
			MPN_COPY(bp, rp, bsize);
		}
		if (rp == ep) {
			/* RES and EXP are identical.  Allocate temp. space for EXP.  */
			ep = ep_marker = oases_mpi_alloc_limb_space(esize);
			if (!ep)
				goto enomem;
			MPN_COPY(ep, rp, esize);
		}
		if (rp == mp) {
			/* RES and MOD are identical.  Allocate temporary space for MOD. */
			BUG_ON(mp_marker);
			mp = mp_marker = oases_mpi_alloc_limb_space(msize);
			if (!mp)
				goto enomem;
			MPN_COPY(mp, rp, msize);
		}
	}

	MPN_COPY(rp, bp, bsize);
	rsize = bsize;
	rsign = bsign;

	{
		mpi_size_t i;
		mpi_ptr_t xp;
		int c;
		mpi_limb_t e;
		mpi_limb_t carry_limb;
		struct karatsuba_ctx karactx;

		xp = xp_marker = oases_mpi_alloc_limb_space(2 * (msize + 1));
		if (!xp)
			goto enomem;

		memset(&karactx, 0, sizeof karactx);
		negative_result = (ep[0] & 1) && base->sign;

		i = esize - 1;
		e = ep[i];
		c = count_leading_zeros(e);
		e = (e << c) << 1;	/* shift the exp bits to the left, lose msb */
		c = BITS_PER_MPI_LIMB - 1 - c;

		/* Main loop.
		 *
		 * Make the result be pointed to alternately by XP and RP.  This
		 * helps us avoid block copying, which would otherwise be necessary
		 * with the overlap restrictions of mpihelp_divmod. With 50% probability
		 * the result after this loop will be in the area originally pointed
		 * by RP (==RES->d), and with 50% probability in the area originally
		 * pointed to by XP.
		 */

		for (;;) {
			while (c) {
				mpi_ptr_t tp;
				mpi_size_t xsize;

				/*if (mpihelp_mul_n(xp, rp, rp, rsize) < 0) goto enomem */
				if (rsize < KARATSUBA_THRESHOLD)
					oases_mpih_sqr_n_basecase(xp, rp, rsize);
				else {
					if (!tspace) {
						tsize = 2 * rsize;
						tspace =
						    oases_mpi_alloc_limb_space(tsize);
						if (!tspace)
							goto enomem;
					} else if (tsize < (2 * rsize)) {
						oases_mpi_free_limb_space(tspace);
						tsize = 2 * rsize;
						tspace =
						    oases_mpi_alloc_limb_space(tsize);
						if (!tspace)
							goto enomem;
					}
					oases_mpih_sqr_n(xp, rp, rsize, tspace);
				}

				xsize = 2 * rsize;
				if (xsize > msize) {
					oases_mpihelp_divrem(xp + msize, 0, xp, xsize,
						       mp, msize);
					xsize = msize;
				}

				tp = rp;
				rp = xp;
				xp = tp;
				rsize = xsize;

				if ((mpi_limb_signed_t) e < 0) {
					/*mpihelp_mul( xp, rp, rsize, bp, bsize ); */
					if (bsize < KARATSUBA_THRESHOLD) {
						mpi_limb_t tmp;
						if (oases_mpihelp_mul
						    (xp, rp, rsize, bp, bsize,
						     &tmp) < 0)
							goto enomem;
					} else {
						if (oases_mpihelp_mul_karatsuba_case
						    (xp, rp, rsize, bp, bsize,
						     &karactx) < 0)
							goto enomem;
					}

					xsize = rsize + bsize;
					if (xsize > msize) {
						oases_mpihelp_divrem(xp + msize, 0,
							       xp, xsize, mp,
							       msize);
						xsize = msize;
					}

					tp = rp;
					rp = xp;
					xp = tp;
					rsize = xsize;
				}
				e <<= 1;
				c--;
			}

			i--;
			if (i < 0)
				break;
			e = ep[i];
			c = BITS_PER_MPI_LIMB;
		}

		/* We shifted MOD, the modulo reduction argument, left MOD_SHIFT_CNT
		 * steps.  Adjust the result by reducing it with the original MOD.
		 *
		 * Also make sure the result is put in RES->d (where it already
		 * might be, see above).
		 */
		if (mod_shift_cnt) {
			carry_limb =
			    oases_mpihelp_lshift(res->d, rp, rsize, mod_shift_cnt);
			rp = res->d;
			if (carry_limb) {
				rp[rsize] = carry_limb;
				rsize++;
			}
		} else {
			MPN_COPY(res->d, rp, rsize);
			rp = res->d;
		}

		if (rsize >= msize) {
			oases_mpihelp_divrem(rp + msize, 0, rp, rsize, mp, msize);
			rsize = msize;
		}

		/* Remove any leading zero words from the result.  */
		if (mod_shift_cnt)
			oases_mpihelp_rshift(rp, rp, rsize, mod_shift_cnt);
		MPN_NORMALIZE(rp, rsize);

		oases_mpihelp_release_karatsuba_ctx(&karactx);
	}

	if (negative_result && rsize) {
		if (mod_shift_cnt)
			oases_mpihelp_rshift(mp, mp, msize, mod_shift_cnt);
		oases_mpihelp_sub(rp, mp, msize, rp, rsize);
		rsize = msize;
		rsign = msign;
		MPN_NORMALIZE(rp, rsize);
	}
	res->nlimbs = rsize;
	res->sign = rsign;

leave:
	rc = 0;
enomem:
	if (assign_rp)
		oases_mpi_assign_limb_space(res, rp, size);
	if (mp_marker)
		oases_mpi_free_limb_space(mp_marker);
	if (bp_marker)
		oases_mpi_free_limb_space(bp_marker);
	if (ep_marker)
		oases_mpi_free_limb_space(ep_marker);
	if (xp_marker)
		oases_mpi_free_limb_space(xp_marker);
	if (tspace)
		oases_mpi_free_limb_space(tspace);
	return rc;
}
