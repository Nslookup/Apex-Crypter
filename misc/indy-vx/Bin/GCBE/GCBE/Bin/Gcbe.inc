; o GCBE
; o U/K, MI
; o (c) Indy, 2011.
;
	include Graph.inc
	
%GPCALL macro Service
	mov eax,Service
	Call GCBE
endm

; GP Services.
;
GP_LDE	equ 0	; ULONG (*PGCBE)(IN PVOID Ip);
GP_PFX	equ 1	; -

GP_PARSE	equ 2

; Flags
GCBE_PARSE_DISCLOSURE	equ 000001B	; �� ������������, ���������� NL.
GCBE_PARSE_SEPARATE		equ 000010B	; ��� ������ ���������� �������� ���������.
GCBE_PARSE_MAKELIST		equ 000100B	; �� ������������ � ������ ������. ������ �������� ������ ��� ����������� �� �����.
GCBE_PARSE_CROSSBREAK	equ 001000B	; �������� ������� �����(��������� �� �� ������ ����������) � �������.
GCBE_PARSE_OPENLIST		equ 010000B	; � ������ �������� ����������� ������ �� CALL_HEADER, ���� ���������� ���� ����, �� �� ������ ���������.
GCBE_PARSE_IPCOUNTING	equ 100000B	; ��������� ����� ���������� � �����. ���� ���� �������, �� ���� IpCount ����� ����������(���������� ��� �������).

; * GCBE_PARSE_CROSSBREAK � GCBE_PARSE_IPCOUNTING �� �������� ��� �������������, ��� ��� ������������ �������� �� ������������������.

; NL
GCBE_PARSE_NL_UNLIMITED	equ -1

comment '
typedef NTSTATUS (*PPARSE_CALLBACK_ROUTINE)(
   IN PVOID *Graph,		// ������ �� ����.
   IN PVOID GraphEntry,	// ������ �� ��������� ����������.
   IN PVOID SubsList,	// ������ ���������� ������ �������� � ������� ������.
   IN ULONG SubsCount,	// ����� �������� � ������ �������� ������� �����������(NL).
   IN BOOLEAN PreOrPost,	// ��� ������.
   IN PVOID Context
   );

 o ������ �������� ����������� ����(EOL).
 o ��������� ������������ ��������� �� ������� ���������: SubsList[0].BranchLink.
 o ��������� ����� ���������� ���� �������. ����������� ������ ��������� ��� ����������� ��������� ������ ��������� �����.
 o �������� ������ ��������� ����� �������������. ������ ��������� � ����� ��� ������� ���������� ���������.
 o ������ ��������� � ������ ����� ���� �� ��������, �� ����� �������� �� ��������� ������.

typedef PVOID (*PSEARCH_CALLBACK_ROUTINE)(
   IN PVOID Address,
   IN PVOID Context,
   IN BOOLEAN FirstSearch
   );

; o ����� ������� ������� ���� ������ ������ ���� �������� ������������ � Gui-����� ����������� PsConvertToGuiThread(), 
;   ���� ������� ������ �������� �������(����� ����������� �� ��������� ���).

typedef NTSTATUS (*PENTRY)(
  IN PVOID Entry,
  IN OUT PVOID *Graph,
  IN ULONG ParseFlags,
  IN ULONG NestingLevel,
  IN PVOID LastIp OPTIONAL,
  IN PPARSE_CALLBACK_ROUTINE ParseCallbackRoutine OPTIONAL,
  IN PVOID ParseCallbackParameter,
  IN PSEARCH_CALLBACK_ROUTINE SearchCallbackRoutine OPTIONAL,
  IN PVOID SearchCallbackParameter
  );
  '

GP_TRACE	equ 3

comment '
typedef NTSTATUS (*PTRACE_CALLBACK_ROUTINE)(
    IN PVOID GpEntry,
    IN PVOID CallbackParameter
    );

typedef NTSTATUS (*PGCBE)(
  IN PVOID Graph,
  IN PTRACE_CALLBACK_ROUTINE CallbackRoutine,
  IN PVOID CallbackParameter
  );
  
 o ����������� ������������ ��������� �� ������� ��������, 
   ��������� �� STATUS_SUCCESS. � ���� ������ ������ ���� 
   �������� ���� ������� ������ ACCESSED_MASK_FLAG �� ����
   ���������� ����� �������� �� �������������, ���� ��� 
   ��������� ��� ������.
  '

GP_MERGE	equ 4

comment '
typedef NTSTATUS (*PGCBE)(
  IN PVOID Src,	; Rw
  IN OUT *Dst,		; Cs
  );
  '

STACK_FRAME struct
Next		PVOID ?	; PSTACK_FRAME
Ip		PVOID ?
STACK_FRAME ends
PSTACK_FRAME typedef ptr STACK_FRAME

GP_CALLER struct
GpEntry		PVOID ?	; 1st
Frame		PSTACK_FRAME ?
SFN			ULONG ?
GP_CALLER ends
PGP_CALLER typedef ptr GP_CALLER

GP_SNAPSHOT struct
Ip		PVOID ?	; ����� ����������� ���������.
GpBase	PVOID ?	; ������� ����� ������ � ������.
GpLimit	PVOID ?	; ����� �����(������ + GpBase).
GP_SNAPSHOT ends
PGP_SNAPSHOT typedef ptr GP_SNAPSHOT

GP_FAST_CHECK_IP_BELONG_TO_SNAPSHOT	equ 5
GP_CHECK_IP_BELONG_TO_SNAPSHOT		equ 6

comment '  
typedef NTSTATUS (*PGCBE)(
  IN PGP_SNAPSHOT Snapshot,
  IN PVOID Ip,
  OUT PVOID GraphEntry
  );
  '

GP_FIND_CALLER_BELONG_TO_SNAPSHOT		equ 7

KernelMode	equ 0
UserMode		equ 1

comment '  
typedef NTSTATUS (*PGCBE)(
  IN PGP_SNAPSHOT Snapshot,
  IN PSTACK_FRAME Sfc,
  IN ULONG Mode,
  OUT PGP_CALLER Caller
  );

 o Mode ���������� �����, ���� ������ KernelMode, �� ����������� 
   ������������� ������(����- ��� ����-) ��� �������� ISR.
  '
 
GP_SEARCH_ROUTINE_ENTRY				equ 8

comment '
typedef PVOID (*PSEARCH_HEAD_CALLBACK)(
   IN PSNAPSHOT_INFORMATION SnapshotInformation,
   IN PVOID GraphEntryForSearch,
   IN PVOID GraphEntryForCheck
   );
   
typedef NTSTATUS (*PGCBE)(
   IN PGP_SNAPSHOT Snapshot,
   IN PVOID Gp OPTIONAL,
   IN PVOID Ip,
   IN ULONG NestingLevel,
   IN PSEARCH_HEAD_CALLBACK SearchCallback OPTIONAL,
   IN PVOID SearchCallbackParameter,
   OUT PCALL_HEADER GraphEntry
   );
   '

GP_QUERY_ROUTINE_ARGS_NUMBER			equ 9

comment '
typedef NTSTATUS (*PGCBE)(
   IN PVOID Graph,
   OUT PULONG ArgsCount
   );
   '

GP_BUILD_GRAPH						equ 10

comment '
typedef NTSTATUS (*PGCBE)(
   IN PVOID GpBase,
   IN PVOID GpLimit,
   IN PVOID CsBase,
   IN PVOID BuildBuffer
   );
   '

GP_SWITCH_THREAD					equ 11

SWT_KERNEL_MODE		equ 000001B
SWT_TRACE				equ 000010B
SWT_ENABLE_ROUTING		equ 000100B
SWT_ROUTE_LAST_FRAME	equ 001000B
SWT_CURRENT_CALLER		equ 010000B

STACK_FRAME_EX struct
Ref		PVOID ?
Sfc		STACK_FRAME <>
STACK_FRAME_EX ends
PSTACK_FRAME_EX typedef ptr STACK_FRAME_EX

comment '
typedef NTSTATUS (*PSET_CONTEXT_ROUTINE)(
   IN PVOID Ip,
   IN PVOID Gp,
   IN PVOID Arg
   );

typedef NTSTATUS (*PFRAME_ENUMERATION_ROUTINE)(
   IN ULONG Mode,
   IN OUT PSTACK_FRAME_EX Frame,
   IN PVOID Arg
   );

typedef NTSTATUS (*PFRAME_LOAD_ROUTINE)(
   IN PSTACK_FRAME Frame,
   IN PVOID Ip,
   IN PVOID Gp,
   IN PVOID Arg
   );

typedef NTSTATUS (*PGCBE)(
  IN PGP_SNAPSHOTPVOID Snapshot,	; Rw-table.
  IN ULONG Flags,
  IN PVOID Ip,
  IN ULONG NestingLevel,
  IN PSEARCH_CALLBACK_ROUTINE SetCtxRoutine,
  IN PVOID SetCtxRoutineArg
  IN PPARSE_CALLBACK_ROUTINE FrEnumRoutine OPTIONAL,
  IN PVOID FrEnumRoutineArg,
  IN PSEARCH_CALLBACK_ROUTINE FrLoadRoutine OPTIONAL,
  IN PVOID FrLoadRoutineArg
  );
  '

GP_REDIRECT_ALL_BRANCH_LINKS	equ 12

comment '
typedef NTSTATUS (*PGCBE)(
   IN PVOID GpBase,
   IN PVOID GpLimit,
   IN PVOID Gp,
   IN PVOID Link
   );
   '

GP_UNLINK_ENTRY			equ 13

comment '
typedef NTSTATUS (*PGCBE)(
   IN PVOID GpBase,
   IN PVOID GpLimit,
   IN PVOID Gp
   );
   '

GP_INSERT_HEAD_ENTRY		equ 14

comment '
typedef NTSTATUS (*PGCBE)(
   IN PVOID GpBase,
   IN PVOID GpLimit,
   IN PVOID CurrentGp,
   IN PVOID NewGp
   );
   '

GCBE::
	include Dump.inc