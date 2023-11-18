	.section ".note","",@note

	# note header

	# length of name
	.long	8

	# note descriptor size
	.long	24

	# note type (IEEE 1275)
	.long	0x1275

	# name of owner
	.asciz	"PowerPC"
	.balign	4


	# note descriptor

	# real mode (-1) or virtual mode (0)
	.long	0

	# real-base
	.long	-1
	# real-size
	.long	-1

	# virt-base
	.long	-1
	# virt-size
	.long	-1
	# load-base 
	.long	0x4000

	# second note is for IBM LPARs
	# length of name
	.long 24
	# note descriptor size
	.long 28
	# note type
	.long 0x12759999
	# name of owner
	.asciz "IBM,RPA-Client-Config"
	.balign 4

	# lpar affinity
	.long 0
	# minimum size in megabytes
	.long 64
	# minimum percentage size
	.long 0
	# max pft size ( 2^48 bytes )
	.long 48
	# splpar
	.long 1
	# min load ?
	.long -1
	# new mem def ?
	.long 0
