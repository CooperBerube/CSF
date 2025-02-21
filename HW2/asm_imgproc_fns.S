/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

	.globl get_r
get_r:
    movl    %edi, %eax  /* Move the value in %edi to %eax */
    shrl    $24, %eax   /* Shift %eax right by 24 bits */
    andl    $0xFF, %eax /* lowest 8 bits */
    ret

	.globl get_g
get_g:
    movl    %edi, %eax      /* Move the value in %edi to %eax */
    shrl    $16, %eax       /* Shift %eax right by 16 bits */
    andl    $0xFF, %eax     /* lowest 8 bits */
    ret                     /* Return the green component */

	.globl get_b
get_b:
    movl    %edi, %eax      /* Move the value in %edi to %eax */
    shrl    $8, %eax        /* Shift %eax right by 8 bits */
    andl    $0xFF, %eax     /* lowest 8 bits */
    ret                     /* Return the blue component */

	.globl get_a
get_a:
    movl    %edi, %eax      /* Move the value in %edi to %eax */
    andl    $0xFF, %eax     /* lowest 8 bits */
    ret                     /* Return the alpha component */

    .globl make_pixel
make_pixel:
    shl $24, %edi       /* Shift r left by 24 bits to occupy bits 24-31 */
    shl $16, %esi       /* Shift g left by 16 bits to occupy bits 16-23 */
    shl $8, %edx        /* Shift b left by 8 bits to occupy bits 8-15 */
    or %esi, %edi       /* Combine g into r */
    or %edx, %edi       /* Combine b into rg */
    or %ecx, %edi       /* Combine a into rgb */
    mov %edi, %eax      /* Move the final result into return register eax */
    ret                 /* Return */

.globl compute_index
compute_index:
    movl 8(%rdi), %eax
    imull %edx, %eax
    addl %esi, %eax
    ret

.globl to_grayscale
to_grayscale:
    pushq   %rbp                /* Save base pointer */
    movq    %rsp, %rbp          /* Set base pointer to stack pointer */

    movl    %edi, %eax          /* Move the pixel value to %eax */
    call    get_r               /* Get the red component */
    movl    %eax, %ebx          /* Save red component in %ebx */

    movl    %edi, %eax          /* Move the pixel value to %eax */
    call    get_g               /* Get the green component */
    movl    %eax, %ecx          /* Save green component in %ecx */

    movl    %edi, %eax          /* Move the pixel value to %eax */
    call    get_b               /* Get the blue component */
    movl    %eax, %edx          /* Save blue component in %edx */

    movl    %edi, %eax          /* Move the pixel value to %eax */
    call    get_a               /* Get the alpha component */
    movl    %eax, %esi          /* Save alpha component in %esi */

    movl    %ebx, %eax          /* Move red component to %eax */
    imull   $79, %eax           /* Multiply red component by 79 */
    movl    %eax, %ebx          /* Save result in %ebx */

    movl    %ecx, %eax          /* Move green component to %eax */
    imull   $128, %eax          /* Multiply green component by 128 */
    addl    %eax, %ebx          /* Add to the result in %ebx */

    movl    %edx, %eax          /* Move blue component to %eax */
    imull   $49, %eax           /* Multiply blue component by 49 */
    addl    %eax, %ebx          /* Add to the result in %ebx */

    shrl    $8, %ebx            /* Shift right by 8 bits to divide by 256 */

    movl    %ebx, %edi          /* Move the grayscale value to %edi */
    movl    %ebx, %esi          /* Move the grayscale value to %esi */
    movl    %ebx, %edx          /* Move the grayscale value to %edx */
    movl    %esi, %ecx          /* Move the alpha value to %ecx */

    call    make_pixel          /* Call make_pixel to create the grayscale pixel */

    movq    %rbp, %rsp          /* Restore stack pointer */
    popq    %rbp                /* Restore base pointer */
    ret                         /* Return the grayscale pixel */

.globl gradient
gradient:
    pushq   %rbp                /* Save base pointer */
    movq    %rsp, %rbp          /* Set base pointer to stack pointer */

    movq    %rdi, %rax          /* Move x to %rax */
    imulq   $2000000000, %rax   /* Multiply x by 2000000000 */
    cqo                         /* Sign extend %rax to %rdx:%rax */
    idivq   %rsi                /* Divide by max, result in %rax */

    movq    $1000000, %rdx      /* Move 1000000 to %rdx */
    imulq   %rdx, %rax          /* Multiply result by 1000000 */
    movq    %rax, %rbx          /* Move result to %rbx */

    subq    $1000, %rbx         /* Subtract 1000 from result */
    imulq   %rbx, %rbx          /* Square the result */
    movq    $1000000, %rax      /* Move 1000000 to %rax */
    subq    %rbx, %rax          /* Subtract squared result from 1000000 */

    /* Ensure the result is positive */
    testq   %rax, %rax          /* Test if the result is negative */
    jns     .L1                 /* If not negative, skip next instruction */
    negq    %rax                /* Negate the result to make it positive */

.L1:
    movq    %rbp, %rsp          /* Restore stack pointer */
    popq    %rbp                /* Restore base pointer */
    ret                         /* Return the result */

/*
 * Implementations of API functions
 */

/*
 * Render an output image containing 4 copies of the original image,
 * refered to as A, B, C, and D in the following diagram:
 *
 *   +---+---+
 *   | A | B |
 *   +---+---+
 *   | C | D |
 *   +---+---+
 *
 * The width and height of the output image are (respectively) twice
 * the width and height of the input image.
 *
 * A is an exact copy of the original input image. B has only the
 * red color component values of the input image, C has only the
 * green color component values, and D has only the blue color component
 * values.
 *
 * Each of the copies (A-D) should use the same alpha values as the
 * original image.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image (which will have
 *          width and height twice the width/height of the
 *          input image)
 */
	.globl imgproc_rgb
imgproc_rgb:
	/* TODO: implement */
    pushq   %rbp                    /* Save base pointer */
    movq    %rsp, %rbp              /* Set base pointer to stack pointer */

    movq    %rdi, %rbx              /* to preserve %rdi and %rsi over calls */
    movq    %rsi, %rcx

    movl    $0, %r10d               /* Set r10d and r11d to 0 for incrementing */
    movl    $0, %r11d               

    movl    IMAGE_WIDTH_OFFSET(%rbx), %r12d     /* Gets W and H and puts in r12d and r13d */
    movl    IMAGE_HEIGHT_OFFSET(%rbx), %r13d    
    movq    IMAGE_DATA_OFFSET(%rbx), %r14       /* Gets data for two pics and puts in r14 and r15 */
    movq    IMAGE_DATA_OFFSET(%rcx), %r15

    subq $16, %rsp

.LrgbOuterLoop:
    cmpl    %r13d, %r11d            /* Compare r13d to r11d */
    jge     .LrgbLoopOuterDone
    .LrgbInnerLoop:
        cmpl    %r12d, %r10d            /* Compare r12d to r10d */
        jge     .LrgbLoopInnerDone

        movl    (%r14,%r10,4), %edi     /* The current pixel goes into %edi for function call */
        call    get_r                   /* Get r value */
        movq    %rax, (%rsp)              /* Save return in r8d */

        movl    (%r14,%r10,4), %edi     /* The current pixel goes into %edi for function call */
        call    get_g                   /* Get g value */
        movq    %rax, 4(%rsp)              /* Save return in r8 */

        movl    (%r14,%r10,4), %edi     /* The current pixel goes into %edi for function call */
        call    get_b                   /* Get b value */
        movq    %rax, 8(%rsp)             /* Save return in r8 */    

        movl    (%r14,%r10,4), %edi     /* The current pixel goes into %edi for function call */
        call    get_a                   /* Get a value */
        movq    %rax, 12(%rsp)          /* Save return in r8 */    


        /* NORMAL */


        movq    (%rsp), %rdi              /* Put ebx into function call register*/
        movq    4(%rsp), %rsi
        movq    8(%rsp), %rdx
        movq    12(%rsp), %rcx

        call    make_pixel 

        movl    %eax, %edi              /* Put pixel into %edi */
        
        movl    %r11d, %r9d             /* Put outer inc into %r9d */
        imull   $2, %r9d                /* Multiply by 2 for double width condition */
        imull   %r12d, %r9d             /* Multiply width by inc and put into %r9d */
        addl    %r10d, %r9d             /* Add inner inc to get specific position */
        movl    (%r14,%r10,4), %edi       /* Put into correct location in data bank for output */
        movl    %edi, (%r15, %r9)


        /* RED */


        /*movl    %ebx, %edi            /* Put ebx into function call registers*/
        /*movl    $0, %esi              /* 0 to g */
        /*movl    $0, %edx              /* 0 to b */
        /*movl    12(%ebx), %ecx        /* a value */

        /*call    make_pixel            /* Make the Pixel */

        /*movl    %eax, %edi            /* Put pixel into %edi */
        
        /*movl    %r11d, (,%r9d,2)           /* Put outer inc into %r9d */
        /*imull   $2, %r9d              /* Multiply by w for double width condition */
        /*imull   %r12d, %r9d           /* Multiply width by inc and put into %r9d */
        /*addl    %r12d, %r9d           /* Add width to get red location
        /*addl    %r10d, %r9d           /* Add inner inc to get specific position */
        /*movl    %edi, (%r15, %r9)     /* Put into correct location in data bank for output */


        /* Green */




        /* Blue */

        incl    %r10d
        jmp     .LrgbInnerLoop
    .LrgbLoopInnerDone:
        incl    %r11d
        movl    $0, %r10d               /* Set r10d to 0 for incrementing next time*/
        jmp     .LrgbOuterLoop
.LrgbLoopOuterDone:
    addq $16, %rsp
    popq    %rbp

	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	/* TODO: implement */
    pushq   %rbp                    /* Save base pointer */
    movq    %rsp, %rbp              /* Set base pointer to stack pointer */

    movq    %rdi, %rbx              /* to preserve %rdi and %rsi over calls */
    movq    %rsi, %rcx

    movl    $0, %r10d               /* Set r10d to zero to use as a index pointer */

    movl    IMAGE_WIDTH_OFFSET(%rbx), %r11d             /* Move width into r11d */
    imull   IMAGE_HEIGHT_OFFSET(%rbx), %r11d           /* Move the height times width into r11d to use as length of data */
    movq    IMAGE_DATA_OFFSET(%rbx), %r12
    movq    IMAGE_DATA_OFFSET(%rcx), %r13
    movq    %rdi, %r8
    movq    %rsi, %r9

.LpixelLoop:
    cmpl    %r11d, %r10d            /* Compare r11d to r10d */
    jae .LpixelLoopDone             /* Jump if r10d is greater than r11d */

    movl    (%r12,%r10,4), %edi    /* The current pixel goes into %edi for function call */

    call    to_grayscale
    movq    %r8, %rdi
    movq    %r9, %rsi
    
    movq    %rax, (%r13,%r10,4)     /* Move the result pixel into new data set of output struct */

    incl %r10d
    jmp .LpixelLoop
.LpixelLoopDone:
    popq    %rbp
	ret



/*
 * Render a "faded" version of the input image.
 *
 * See the assignment description for an explanation of how this transformation
 * should work.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image
 */
	.globl imgproc_fade
imgproc_fade:
	/* TODO: implement */
	ret

/*
 * Render a "kaleidoscope" transformation of input_img in output_img.
 * The input_img must be square, i.e., the width and height must be
 * the same. Assume that the input image is divided into 8 "wedges"
 * like this:
 *
 *    +----+----+
 *    |\ A |   /|
 *    | \  |  / |
 *    |  \ | /  |
 *    | B \|/   |
 *    +----+----+
 *    |   /|\   |
 *    |  / | \  |
 *    | /  |  \ |
 *    |/   |   \|
 *    +----+----+
 *
 * The wedge labeled "A" is the one that will be replicated 8 times
 * in the output image. In the output image,
 *
 *    Wedge A will have exactly the same pixels as wedge A in
 *    the input image, in the same positions.
 *
 *    Wedge B will have exactly the same pixels as wedge A, but the
 *    column and row are transposed. (I.e., it is a reflection across
 *    the diagonal.)
 *
 *    All other wedges are mirrored versions of A such that the overall
 *    image is symmetrical across all four lines dividing the image
 *    (vertical split, horizontal split, and the two diagonal splits.)
 *
 * Parameters:
 *   input_img  - pointer to the input Image
 *   output_img - pointer to the output Image (in which the transformed
 *                pixels should be stored)
 *
 * Returns:
 *   1 if successful, 0 if the transformation fails because the
 *   width and height of input_img are not the same.
 */
	.globl imgproc_kaleidoscope
imgproc_kaleidoscope:
	/* TODO: implement */
	ret

	/* This avoids linker warning about executable stack */
.section .note.GNU-stack,"",@progbits

/*
vim:ft=gas:
*/