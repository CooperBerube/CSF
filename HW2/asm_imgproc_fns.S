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
    movl IMAGE_WIDTH_OFFSET(%rdi), %eax
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

    movl    %esi, %r8d

    movl    %ebx, %edi          /* Move the grayscale value to %edi */
    movl    %ebx, %esi          /* Move the grayscale value to %esi */
    movl    %ebx, %edx          /* Move the grayscale value to %edx */
    movl    %r8d, %ecx          /* Move the alpha value to %ecx */

    call    make_pixel          /* Call make_pixel to create the grayscale pixel */

    movq    %rbp, %rsp          /* Restore stack pointer */
    popq    %rbp                /* Restore base pointer */
    ret                         /* Return the grayscale pixel */

.globl gradient
gradient:
    /* TODO: fix this for working fade */
    pushq %rbp                    /* Save base pointer */
    movq %rsp, %rbp               /* Set base pointer to stack pointer */
    pushq %rbx                    /* Save %rbx register */
    pushq %r10                    /* Save %r10 register */
    pushq %r11                    /* Save %r11 register */

    movq %rdi, %rbx               /* Move first argument (numerator) to %rbx */
    movq %rsi, %r10               /* Move second argument (denominator) to %r10 */
    movq $1000, %r11              /* Move constant 1000 to %r11 */
    xorq %rax, %rax               /* Clear %rax register */

    movq $2000, %rax              /* Move constant 2000 to %rax */
    imulq %rbx                    /* Multiply %rbx (numerator) by %rax */
    idivq %r10                    /* Divide %rax by %r10 (denominator) */

    subq %r11, %rax               /* Subtract 1000 (stored in %r11) from %rax */
    movq %rax, %rdx               /* Move result to %rdx */
    imulq %rdx, %rdx              /* Square %rdx */
    movq $1000000, %rax           /* Move constant 1000000 to %rax */
    subq %rdx, %rax               /* Subtract %rdx from %rax */

    popq %r11                     /* Restore %r11 register */
    popq %r10                     /* Restore %r10 register */
    popq %rbx                     /* Restore %rbx register */
    movq %rbp, %rsp               /* Restore stack pointer */
    popq %rbp                     /* Restore base pointer */

    ret                           /* Return from function */

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
    pushq   %rbp                    /* Save base pointer */
    movq    %rsp, %rbp              /* Set base pointer to stack pointer */

    movq    %rdi, %rbx              /* to preserve %rdi and %rsi over calls */
    movq    %rsi, %rcx

    movl    $0, %r10d               /* Set r10d and r11d to 0 for incrementing */
    movl    $0, %r11d               

    movl    IMAGE_WIDTH_OFFSET(%rdi), %r12d     /* Gets W and H and puts in r12d and r13d */
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r13d    
    movq    IMAGE_DATA_OFFSET(%rdi), %r14       /* Gets data for two pics and puts in r14 and r15 */
    movq    IMAGE_DATA_OFFSET(%rsi), %r15


.LrgbOuterLoop:
    cmpl    %r13d, %r11d            /* Compare r13d to r11d */
    jae     .LrgbLoopOuterDone
    .LrgbInnerLoop:
        cmpl    %r12d, %r10d            /* Compare r12d to r10d */
        jae     .LrgbLoopInnerDone


        /* NORMAL */

        
        movq    %r11, %r8           /* Puts the y or outer increment into %r8 */
        imulq   %r12, %r8           /* Multiply the outerincrement by the total width to get to the point it is held in data */
        addq    %r10, %r8           /* Add the current increment to it to find the column it is int */
        imulq   $4, %r8             /* Multiply by four for space for memory */
        movl    (%r14, %r8), %r8d   /* Get the value from input picture and store it in r8d */

        movq    %r11, %r9           /* Put the y or outer increment into %r9 */
        imulq   %r12, %r9           /* Multiply the outer increment by the width to go to memory location */
        imulq   $2, %r9             /* Multiply by 2 since this is double width */
        addq    %r10, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */


        /* RED */


        movl    %r8d, %edi          /* The current pixel goes into %edi for function call */
        call    get_a               /* Get a value */
        movl    %eax, %ecx          /* Save return in ecx for future func call */   

        movl    %r8d, %edi          /* The current pixel goes into %edi for function call */
        call    get_r               /* Get r value */
        movl    %eax, %edi          /* Save return in %edi */

        movl    $0, %esi            /* Set g and b components to 0 */
        movl    $0, %edx

        call    make_pixel          /* Create the pixel */
        
        movq    %r11, %r9           /* Put Outer increment into %r9 for y location*/
        imulq   %r12, %r9           /* Multiply by width */
        imulq   $2, %r9             /* Multiply by 2 for double size */
        addq    %r10, %r9           /* Add inner loop increment for x location */
        addq    %r12, %r9           /* Add width for red quadrant specific location */
        imulq   $4, %r9             /* Multiply by four for byte size */
        movl    %eax, (%r15,%r9)    /* Put pixel made from make_pixel into spot */


        /* Green */


        movl    %r8d, %edi          /* The current pixel goes into %edi for function call */
        call    get_g               /* Get g value */
        movl    %eax, %esi          /* Save return in %esi */

        movl    $0, %edi            /* Set r and b components to 0 */
        movl    $0, %edx

        call    make_pixel          /* Create the pixel */
        
        movq    %r11, %r9           /* Put Outer increment into %r9 for y location*/
        addq    %r13, %r9           /* Add height to get quadrant specific to green */
        imulq   %r12, %r9           /* Multiply by width */
        imulq   $2, %r9             /* Multiply by 2 for double size */
        addq    %r10, %r9           /* Add inner loop increment for x location */
        imulq   $4, %r9             /* Multiply by four for byte size */
        movl    %eax, (%r15,%r9)    /* Put pixel made from make_pixel into spot */
        

        /* Blue */


        movl    %r8d, %edi          /* The current pixel goes into %edi for function call */
        call    get_b               /* Get b value */
        movl    %eax, %edx          /* Save return in %edx */

        movl    $0, %edi            /* Set r and g components to 0 */
        movl    $0, %esi

        call    make_pixel          /* Create the pixel */
        
        movq    %r11, %r9           /* Put Outer increment into %r9 for y location*/
        addq    %r13, %r9           /* Bottom quadrant height condition */
        imulq   %r12, %r9           /* Multiply by width */
        imulq   $2, %r9             /* Multiply by 2 for double size */
        addq    %r10, %r9           /* Add inner loop increment for x location */
        addq    %r12, %r9           /* Add width for blue quadrant specific location */
        imulq   $4, %r9             /* Multiply by four for byte size */
        movl    %eax, (%r15,%r9)    /* Put pixel made from make_pixel into spot */

        /* Loop finishing step */

        incl    %r10d               /* increment %r10 */
        jmp     .LrgbInnerLoop
    .LrgbLoopInnerDone:
        incl    %r11d               /* increment %r11 */
        movl    $0, %r10d           /* Set r10d to 0 for incrementing next time*/
        jmp     .LrgbOuterLoop
.LrgbLoopOuterDone:
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
    imull   IMAGE_HEIGHT_OFFSET(%rbx), %r11d            /* Move the height times width into r11d to use as length of data */
    movq    IMAGE_DATA_OFFSET(%rbx), %r12               /* Store input and ouput data */
    movq    IMAGE_DATA_OFFSET(%rcx), %r13

.LpixelLoop:
    cmpl    %r11d, %r10d            /* Compare r11d to r10d */
    jae .LpixelLoopDone             /* Jump if r10d is greater than r11d */

    movl    (%r12,%r10,4), %edi    /* The current pixel goes into %edi for function call */

    movq    $0, %rax

    call    to_grayscale

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
    pushq   %rbp                    // Save base pointer
    movq    %rsp, %rbp              // Set base pointer to stack pointer
    subq    $32, %rsp               // Allocate 32 bytes on the stack
    pushq   %r15                    // Save %r15 register
    pushq   %r14                    // Save %r14 register
    pushq   %r13                    // Save %r13 register
    pushq   %r12                    // Save %r12 register
    pushq   %rbx                    // Save %rbx register

    // Check if input_img or output_img is NULL
    testq   %rdi, %rdi              // Test if %rdi (input_img) is NULL
    je      .Lfade_return           // If NULL, jump to .Lfade_return
    testq   %rsi, %rsi              // Test if %rsi (output_img) is NULL
    je      .Lfade_return           // If NULL, jump to .Lfade_return

    // Load image dimensions and data pointers
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r12d  // Load input_img->height into %r12d
    movl    IMAGE_WIDTH_OFFSET(%rdi), %r13d   // Load input_img->width into %r13d
    movq    IMAGE_DATA_OFFSET(%rdi), %r14     // Load input_img->data into %r14
    movq    IMAGE_DATA_OFFSET(%rsi), %r15     // Load output_img->data into %r15

    xorl    %r10d, %r10d            // Initialize row index %r10d to 0

.Lfade_outer_loop:
    cmpl    %r12d, %r10d            // Compare row index %r10d with height %r12d
    je      .Lfade_outer_loop_done  // If equal, jump to .Lfade_outer_loop_done

    xorl    %r11d, %r11d            // Initialize column index %r11d to 0

.Lfade_inner_loop:
    cmpl    %r13d, %r11d            // Compare column index %r11d with width %r13d
    je      .Lfade_inner_loop_done  // If equal, jump to .Lfade_inner_loop_done

    // Calculate the pixel index
    movl    %r10d, %r9d             // Copy row index %r10d to %r9d
    imull   %r13d, %r9d             // Multiply row index by width to get row offset
    addl    %r11d, %r9d             // Add column index to get pixel index

    // Load the pixel value
    movl    (%r14, %r9, 4), %eax    // Load pixel value from input_img->data into %eax
    movl    %eax, -4(%rbp)          // Store pixel value on the stack
    movl    %r9d, -24(%rbp)         // Store pixel index on the stack

    // Calculate the gradient for the row
    movl    %r10d, %edi             // Move row index to %edi (first argument)
    movl    %r12d, %esi             // Move height to %esi (second argument)
    call    gradient                // Call gradient function
    movq    %rax, -32(%rbp)         // Store gradient result on the stack

    // Calculate the gradient for the column
    movl    %r11d, %edi             // Move column index to %edi (first argument)
    movl    %r13d, %esi             // Move width to %esi (second argument)
    call    gradient                // Call gradient function
    movq    %rax, %rbx              // Store gradient result in %rbx
    imulq   -32(%rbp), %rbx         // Multiply row gradient by column gradient

    // Extract and fade the red component
    movl    -4(%rbp), %edi          // Load pixel value from the stack into %edi
    call    get_r                   // Call get_r function to extract red component
    imulq   %rbx, %rax              // Multiply red component by gradient product
    movq    $1000000000000, %r8     // Load constant 1000000000000 into %r8
    xorq    %rdx, %rdx              // Clear %rdx
    divq    %r8                     // Divide by 1000000000000
    movl    %eax, -8(%rbp)          // Store faded red component on the stack

    // Extract and fade the green component
    movl    -4(%rbp), %edi          // Load pixel value from the stack into %edi
    call    get_g                   // Call get_g function to extract green component
    imulq   %rbx, %rax              // Multiply green component by gradient product
    xorq    %rdx, %rdx              // Clear %rdx
    divq    %r8                     // Divide by 1000000000000
    movl    %eax, -12(%rbp)         // Store faded green component on the stack

    // Extract and fade the blue component
    movl    -4(%rbp), %edi          // Load pixel value from the stack into %edi
    call    get_b                   // Call get_b function to extract blue component
    imulq   %rbx, %rax              // Multiply blue component by gradient product
    xorq    %rdx, %rdx              // Clear %rdx
    divq    %r8                     // Divide by 1000000000000
    movl    %eax, -16(%rbp)         // Store faded blue component on the stack

    // Extract the alpha component
    movl    -4(%rbp), %edi          // Load pixel value from the stack into %edi
    call    get_a                   // Call get_a function to extract alpha component
    movl    %eax, -20(%rbp)         // Store alpha component on the stack

    // Combine the faded components into a new pixel
    movl    -8(%rbp), %edi          // Load faded red component into %edi
    movl    -12(%rbp), %esi         // Load faded green component into %esi
    movl    -16(%rbp), %edx         // Load faded blue component into %edx
    movl    -20(%rbp), %ecx         // Load alpha component into %ecx
    call    make_pixel              // Call make_pixel function

    // Store the new pixel value
    movl    %eax, %ebx              // Move new pixel value to %ebx
    movl    -24(%rbp), %r9d         // Load pixel index from the stack into %r9d
    movl    %ebx, (%r15, %r9, 4)    // Store new pixel value in output_img->data

    // Increment the column index
    incl    %r11d                   // Increment column index
    jmp     .Lfade_inner_loop       // Jump to the start of the inner loop

.Lfade_inner_loop_done:
    incl    %r10d                   // Increment row index
    jmp     .Lfade_outer_loop       // Jump to the start of the outer loop

.Lfade_outer_loop_done:
    popq    %rbx                    // Restore %rbx register
    popq    %r12                    // Restore %r12 register
    popq    %r13                    // Restore %r13 register
    popq    %r14                    // Restore %r14 register
    popq    %r15                    // Restore %r15 register
    addq    $32, %rsp               // Deallocate 32 bytes from the stack
    popq    %rbp                    // Restore base pointer
    ret                             // Return from function

.Lfade_return:
    popq    %rbx                    // Restore %rbx register
    popq    %r12                    // Restore %r12 register
    popq    %r13                    // Restore %r13 register
    popq    %r14                    // Restore %r14 register
    popq    %r15                    // Restore %r15 register
    addq    $32, %rsp               // Deallocate 32 bytes from the stack
    popq    %rbp                    // Restore base pointer
    ret                             // Return from function

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
    pushq   %rbp                    /* Save base pointer */
    movq    %rsp, %rbp              /* Set base pointer to stack pointer */

    movq    %rdi, %rbx              /* to preserve %rdi and %rsi over calls */
    movq    %rsi, %rcx

    movl    $0, %r10d               /* Set r10d and r11d to 0 for incrementing */
    movl    $0, %r11d               


    movl    IMAGE_WIDTH_OFFSET(%rdi), %r12d     /* Gets W and H and puts in r12d and r13d */
    shr    $1, %r12
    movl    IMAGE_WIDTH_OFFSET(%rdi), %edx     /* Gets W and H and puts in r12d and r13d */
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r13d  

    movq    IMAGE_DATA_OFFSET(%rdi), %r14       /* Gets data for two pics and puts in r14 and r15 */
    movq    IMAGE_DATA_OFFSET(%rsi), %r15

    cmpl    %edx, %r13d
    jz      .Lcontinue
    movl    $0, %eax
    ret
    .Lcontinue:

    movq    $0, %rdx
    movl    %r13d, %eax
    movl    $2, %ecx
    divq    %rcx
    cmpq    $1, %rdx
    jz      .Lodd

    movl    IMAGE_WIDTH_OFFSET(%rdi), %edx     /* Gets W and H and puts in r12d and r13d */
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r13d  

    


.LkaleidoscopeOuterLoop:
    cmpl    %r12d, %r11d            /* Compare r12d to r11d */
    jae     .LkaleidoscopeLoopOuterDone
    movl %r11d, %r10d               /* Move y into x to get proper part of input image */
    .LkaleidoscopeInnerLoop:
        cmpl    %r12d, %r10d            /* Compare r12d to r10d */
        jae     .LkaleidoscopeLoopInnerDone

        /* Get picture index */

        movl    %edx, %r8d          /* Puts the width into %r8 */
        imulq   %r11, %r8           /* Multiply the outerincrement by the total width to get to the point it is held in data */
        
        addq    %r10, %r8           /* Add the current increment to it to find the column it is int */
        imulq   $4, %r8             /* Multiply by four for space for memory */
        movl    (%r14, %r8), %r8d   /* Get the value from input picture and store it in r8d */

        /* top Left top*/

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %r11, %r9           /* Multiply the outer increment by the width to go to memory location */
        
        addq    %r10, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top Left bottom*/

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %r10, %r9           /* Multiply the inner increment by the width to go to memory location */
        
        addq    %r11, %r9           /* Add r11 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom Left top */ 

        
        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rcx, %r9           /* Multiply the inner increment by the width to go to memory location */
        
        addq    %r11, %r9           /* Add r11 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom Left bottom */
        
        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %r9d
        imulq   %rcx, %r9           /* Multiply the outer increment by the width to go to memory location */
        
        addq    %r10, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top right top */

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %r9d          /* Put the y or outer increment into %r9 */
        imulq   %r11, %r9           /* Multiply the outer increment by the width to go to memory location */

        addq    %rcx, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top right bottom */

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %r10, %r9           /* Multiply the inner increment by the width to go to memory location */

        addq    %rcx, %r9           /* Add adjusted increment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom right top */

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %ebx
        subl    $1, %ebx
        subl    %r10d, %ebx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rbx, %r9           /* Multiply by adjusted increment to go to memory location */
        
        addq    %rcx, %r9           /* Add adjusted increment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom right bottom */

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %ebx
        subl    $1, %ebx
        subl    %r11d, %ebx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rbx, %r9           /* Multiply by adjusted increment to go to memory location */
        
        addq    %rcx, %r9           /* Add adjusted increment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* Loop finishing step */

        incl    %r10d               /* increment %r10 */
        jmp     .LkaleidoscopeInnerLoop
    .LkaleidoscopeLoopInnerDone:
        incl    %r11d               /* increment %r11 */
        jmp     .LkaleidoscopeOuterLoop
.LkaleidoscopeLoopOuterDone:
    movl    $1, %eax
    popq    %rbp
	ret

.Lodd:

    movl    IMAGE_WIDTH_OFFSET(%rdi), %edx     /* Gets W and H and puts in r12d and r13d */
    movl    IMAGE_HEIGHT_OFFSET(%rdi), %r13d  
    movq    $0, %rdi
    addl    $1, %r12d

.LkaleidoscopeOddOuterLoop:
    cmpl    %r12d, %r11d            /* Compare r12d to r11d */
    jae     .LkaleidoscopeLoopOddOuterDone
    movl %r11d, %r10d               /* Move y into x to get proper part of input image */
    .LkaleidoscopeOddInnerLoop:
        cmpl    %r12d, %r10d            /* Compare r12d to r10d */
        jae     .LkaleidoscopeLoopOddInnerDone

        /* Get picture index */

        movl    %edx, %r8d          /* Puts the width or outer increment into %r8 */
        imulq   %r11, %r8           /* Multiply the outerincrement by the total width to get to the point it is held in data */
        
        addq    %r10, %r8           /* Add the current increment to it to find the column it is int */
        imulq   $4, %r8             /* Multiply by four for space for memory */
        movl    (%r14, %r8), %r8d   /* Get the value from input picture and store it in r8d */

        /* top Left top*/

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %r11, %r9           /* Multiply the outer increment by the width to go to memory location */
        
        addq    %r10, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top Left bottom*/

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %r10, %r9           /* Multiply the inner increment by the width to go to memory location */
        
        addq    %r11, %r9           /* Add r11 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom Left top */ 

        
        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rcx, %r9           /* Multiply the inner increment by the width to go to memory location */
        
        addq    %r11, %r9           /* Add r11 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom Left bottom */

        cmpl    %r11d, %edi         /* Comparisons to handle odd conditions */
        jz      .LloopFinishingSteps

        cmpl    %r10d, %edi
        jz      .LloopFinishingSteps
        
        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %r9d
        imulq   %rcx, %r9           /* Multiply the outer increment djustment by the width to go to memory location */
        
        addq    %r10, %r9           /* Add r10 to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top right top */

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %r9d          /* Put the y or outer increment into %r9 */
        imulq   %r11, %r9           /* Multiply the outer increment by the width to go to memory location */

        addq    %rcx, %r9           /* Add inner increment adjustment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* top right bottom */

        cmpl    %r11d, %edi         /* Comparisons to handle odd conditions */
        jz      .LloopFinishingSteps

        cmpl    %r10d, %edi
        jz      .LloopFinishingSteps

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %r9d          /* Put width into %r9 */
        imulq   %r10, %r9           /* Multiply the inner increment by the width to go to memory location */

        addq    %rcx, %r9           /* Add r11 adjustment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom right top */

        cmpl    %r11d, %edi         /* Comparisons to handle odd conditions */
        jz      .LloopFinishingSteps

        cmpl    %r10d, %edi
        jz      .LloopFinishingSteps

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r11d, %ecx

        movl    %edx, %ebx
        subl    $1, %ebx
        subl    %r10d, %ebx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rbx, %r9           /* Multiply the inner increment adjustment by the width to go to memory location */
        
        addq    %rcx, %r9           /* Add outer increment adjustment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* bottom right bottom */

        cmpl    %r11d, %edi         /* Comparisons to handle odd conditions */
        jz      .LloopFinishingSteps

        cmpl    %r10d, %edi
        jz      .LloopFinishingSteps

        movl    %edx, %ecx          /* adjust increments for quadrant location */
        subl    $1, %ecx
        subl    %r10d, %ecx

        movl    %edx, %ebx
        subl    $1, %ebx
        subl    %r11d, %ebx

        movl    %edx, %r9d          /* Put the width into %r9 */
        imulq   %rbx, %r9           /* Multiply the outer increment by the width to go to memory location */
        
        addq    %rcx, %r9           /* Add inner increment adjustment to get to column location */
        imulq   $4, %r9             /* Multiply by four for space for memory */
        movl    %r8d, (%r15,%r9)    /* Take original memory location and put into new memory location */

        /* Loop finishing step */
        .LloopFinishingSteps:

        incl    %r10d               /* increment %r10 */
        jmp     .LkaleidoscopeOddInnerLoop
    .LkaleidoscopeLoopOddInnerDone:
        incl    %r11d               /* increment %r11 */
        jmp     .LkaleidoscopeOddOuterLoop
.LkaleidoscopeLoopOddOuterDone:
    movl    $1, %eax
    popq    %rbp
	ret

	/* This avoids linker warning about executable stack */
.section .note.GNU-stack,"",@progbits

/*
vim:ft=gas:
*/