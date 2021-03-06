/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      Inline functions (gcc style 386 asm).
 *
 *      By Shawn Hargreaves.
 *
 *      See readme.txt for copyright information.
 */


#if (!defined ALLEGRO_GCC) || (!defined ALLEGRO_I386)
   #error bad include
#endif



/* _default_ds:
 *  Return a copy of the current %ds selector.
 */
AL_INLINE(int, _default_ds, (void),
{
   short result;

   asm (
      "  movw %%ds, %0 "

   : "=r" (result)
   );

   return result;
})



/* bmp_write_line:
 *  Bank switch function.
 */
AL_INLINE(unsigned long, bmp_write_line, (BITMAP *bmp, int line),
{
   unsigned long result;

   asm volatile (
      "  call *%3 "

   : "=a" (result)                     /* result in eax */

   : "d" (bmp),                        /* bitmap in edx */
     "0" (line),                       /* line number in eax */
     "r" (bmp->write_bank)             /* the bank switch routine */
   );

   return result;
})



/* bmp_read_line:
 *  Bank switch function.
 */
AL_INLINE(unsigned long, bmp_read_line, (BITMAP *bmp, int line),
{
   unsigned long result;

   asm volatile (
      "  call *%3 "

   : "=a" (result)                     /* result in eax */

   : "d" (bmp),                        /* bitmap in edx */
     "0" (line),                       /* line number in eax */
     "r" (bmp->read_bank)              /* the bank switch routine */
   );

   return result;
})



/* bmp_unwrite_line:
 *  Terminate bank switch function.
 */
AL_INLINE(void, bmp_unwrite_line, (BITMAP *bmp),
{
   asm volatile (
      "  call *%1 "
   :
   : "d" (bmp),                        /* bitmap in edx */
     "r" (bmp->vtable->unwrite_bank)   /* the bank switch routine */
   );
})



/* Helper macro that makes the compiler reduce fadd(), fsub(), fmul() and
   fdiv() calls to a single constant if both operands are constant. Since
   this doesn't work unless we compile with optimization, it's better to skip
   the test then. */
#if (defined __OPTIMIZE__) && ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 95)))
   #define __PRECALCULATE_CONSTANTS(calc)                                  \
      if(__builtin_constant_p(x) && __builtin_constant_p(y)) {             \
	 if((calc) > (double)0x7FFFFFFF) {                                 \
	    *allegro_errno = ERANGE;                                       \
	    return 0x7FFFFFFF;                                             \
	 }                                                                 \
	 else if(-(calc) > (double)0x7FFFFFFF) {                           \
	    *allegro_errno = ERANGE;                                       \
	    return -0x7FFFFFFF;                                            \
	 }                                                                 \
	 else                                                              \
	    return (fixed)(calc);                                          \
      }                                                                    \
      else
#else
   #define __PRECALCULATE_CONSTANTS(calc)
#endif



/* fadd:
 *  Fixed point (16.16) addition.
 */
AL_INLINE(fixed, fadd, (fixed x, fixed y),
{
   fixed result;

   __PRECALCULATE_CONSTANTS(x + (double)y)
   {
      asm (
	 "  addl %2, %0 ; "               /* do the addition */
	 "  jno 0f ; "                    /* check for overflow */

	 "  movl %4, %0 ; "               /* on overflow, set errno */
	 "  movl %3, (%0) ; "
	 "  movl $0x7FFFFFFF, %0 ; "      /* and return MAXINT */
	 "  cmpl $0, %2 ; "
	 "  jg 0f ; "
	 "  negl %0 ; "

	 " 0: "                           /* finished */

      : "=r" (result)                     /* result in a register */

      : "0" (x),                          /* x in the output register */
	"rm" (y),                         /* y can go in register or memory */
	"i" (ERANGE),
	"m" (allegro_errno)

      : "%cc", "memory"                   /* clobbers flags and errno */
      );

      return result;
   }
})



/* fsub:
 *  Fixed point (16.16) subtraction.
 */
AL_INLINE(fixed, fsub, (fixed x, fixed y),
{
   fixed result;

   __PRECALCULATE_CONSTANTS(x - (double)y)
   {
      asm (
	 "  subl %2, %0 ; "               /* do the subtraction */
	 "  jno 0f ; "                    /* check for overflow */

	 "  movl %4, %0 ; "               /* on overflow, set errno */
	 "  movl %3, (%0) ; "
	 "  movl $0x7FFFFFFF, %0 ; "      /* and return MAXINT */
	 "  cmpl $0, %2 ; "
	 "  jl 0f ; "
	 "  negl %0 ; "

	 " 0: "                           /* finished */

      : "=r" (result)                     /* result in a register */

      : "0" (x),                          /* x in the output register */
	"rm" (y),                         /* y can go in register or memory */
	"i" (ERANGE),
	"m" (allegro_errno)

      : "%cc", "memory"                   /* clobbers flags and errno */
      );

      return result;
   }
})



/* fmul:
 *  Fixed point (16.16) multiplication.
 */
AL_INLINE(fixed, fmul, (fixed x, fixed y),
{
   fixed edx __attribute__ ((__unused__));
   fixed result;

   __PRECALCULATE_CONSTANTS(x / 65536.0 * y)
   {
      asm (
	 "  movl %2, %%eax ; "
	 "  imull %3 ; "                  /* do the multiply */
	 "  shrdl $16, %%edx, %%eax ; "

	 "  sarl $15, %%edx ; "           /* check for overflow */
	 "  jz 0f ; "
	 "  cmpl $-1, %%edx ; "
	 "  je 0f ; "

	 "  movl %5, %%eax ; "            /* on overflow, set errno */
	 "  movl %4, (%%eax) ; "
	 "  movl $0x7FFFFFFF, %%eax ; "   /* and return MAXINT */
	 "  cmpl $0, %2 ; "
	 "  jge 1f ; "
	 "  negl %%eax ; "
	 " 1: "
	 "  cmpl $0, %3 ; "
	 "  jge 0f ; "
	 "  negl %%eax ; "

	 "  .balign 4, 0x90 ; "

	 " 0: "                           /* finished */

      : "=&a" (result),                   /* the result has to go in eax */
	"=&d" (edx)                       /* reliably reserve edx */

      : "mr" (x),                         /* x and y can be regs or mem */
	"mr" (y),
	"i" (ERANGE),
	"m" (allegro_errno)

      : "%cc", "memory"                   /* clobbers flags and errno */
      );

      return result;
   }
})



/* fdiv:
 *  Fixed point (16.16) division.
 */
AL_INLINE(fixed, fdiv, (fixed x, fixed y),
{
   fixed edx __attribute__ ((__unused__));
   fixed reg __attribute__ ((__unused__));
   fixed result;

   __PRECALCULATE_CONSTANTS(x * 65536.0 / y)
   {
      asm (
	 "  testl %%eax, %%eax ; "        /* test sign of x */
	 "  js 3f ; "

	 "  testl %2, %2 ; "              /* test sign of y */
	 "  jns 4f ; "
	 "  negl %2 ; "

	 " 0: "                           /* result will be negative */
	 "  movl %%eax, %%edx ; "         /* check the range is ok */
	 "  shrl $16, %%edx ; "
	 "  shll $16, %%eax ; "
	 "  cmpl %2, %%edx ; "
	 "  jae 1f ; "

	 "  divl %2 ; "                   /* do the divide */
	 "  testl %%eax, %%eax ; "
	 "  jns 2f ; "

	 " 1: "
	 "  movl %6, %%eax ; "            /* on overflow, set errno */
	 "  movl %5, (%%eax) ; "
	 "  movl $0x7FFFFFFF, %%eax ; "   /* and return MAXINT */

	 " 2: "
	 "  negl %%eax ; "                /* fix up the sign of the result */
	 "  jmp 6f ; "

	 "  .balign 4, 0x90 ; "

	 " 3: "                           /* x is negative */
	 "  negl %%eax ; "
	 "  testl %2, %2 ; "              /* test sign of y */
	 "  jns 0b ; "
	 "  negl %2 ; "

	 " 4: "                           /* result will be positive */
	 "  movl %%eax, %%edx ; "         /* check the range is ok */
	 "  shrl $16, %%edx ; "
	 "  shll $16, %%eax ; "
	 "  cmpl %2, %%edx ; "
	 "  jae 5f ; "

	 "  divl %2 ; "                   /* do the divide */
	 "  testl %%eax, %%eax ; "
	 "  jns 6f ; "

	 " 5: "
	 "  movl %6, %%eax ; "            /* on overflow, set errno */
	 "  movl %5, (%%eax) ; "
	 "  movl $0x7FFFFFFF, %%eax ; "   /* and return MAXINT */

	 " 6: "                           /* finished */

      : "=a" (result),                    /* the result has to go in eax */
	"=&d" (edx),                      /* reliably reserve edx */
	"=r" (reg)                        /* input operand will be clobbered */

      : "0" (x),                          /* x in eax */
	"2" (y),                          /* y in register */
	"i" (ERANGE),
	"m" (allegro_errno)

      : "%cc", "memory"                   /* clobbers flags and memory  */
      );

      return result;
   }
})



/* fceil :
 * Fixed point version of ceil().
 * Note that it returns an integer result (not a fixed one)
 */
AL_INLINE(int, fceil, (fixed x),
{
   int result;

   asm (
      " addl $0xFFFF, %0 ;"	/* ceil () */
      " jns 0f ;"
      " jo 1f ;"

      "0:"
      " sarl $16, %0 ;"		/* convert to int */
      " jmp 2f ;"

      "1:"
      " movl %3, %0 ;"		/* on overflow, set errno */
      " movl %2, (%0) ;"
      " movl $0x7FFF, %0 ;"	/* and return large int */

      "2:"
    : "=r" (result)		/* result in a register */

    : "0" (x),			/* x in the output register */
      "i" (ERANGE),
      "m" (allegro_errno)

    : "%cc", "memory"		/* clobbers flags and errno */
   );

   return result;
})



#undef __PRECALCULATE_CONSTANTS

