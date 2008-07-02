.text

#
# real bkpt function
# this function calls arm bkpt instruction
# this instruction is not doing anything on device
# prior to av400 bkpt instruction was not catch by arm
# so it could be safely left in the code
# aomulator is intrepreting bkpt as a pause in the code 
# execution.
# for device like av400 or later, medios should be build
# with bkp_dummy.c (HAVE_DBUG define in Makefile set to 0)
#

.global do_bkpt
do_bkpt:
    bkpt 0
    mov PC, LR
    