; output from simplelang compiler

; declare a->address 0
; declare b->address 1
; declare c->address 2

ldi A10
mov M0
ldi A20
mov M1
mov A M0
mov B M1
add
mov M A2
mov A M2
ldi B30
cmp
jne end_if0
mov A M2
ldi B1
add
mov M A2
end_if_0:
hlt
