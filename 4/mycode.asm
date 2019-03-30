org 100h
jmp start 
   
FirstTerm db 'enter the first term (max five digits): ','$'
SecondTerm db 'enter the second term (max five digits): ','$'
CalcSum db 'sum = ','$'    
NewLine db 0Dh,0AH,"$"  
inA db  5  dup(0)
inB db  5  dup(0)  

N EQU 5


start:
main PROC        
             
   lea dx, FirstTerm            
   call output         

   lea di, inA
   call input    

   mov ax, N
   sub ax, cx     
   cmp ax, 5 ;check the input whether it is letter or digit
   JE WRITE_B   
   mov cx, ax 
   lea si, inA;
   add si, ax
   sub si, 1
   lea di, inA + N-1
   call calc_shift
            
WRITE_B:  
   lea dx, NewLine            
   call output    

 ;  lea si, inA 
 ;  call output_array   
   
 ;  lea dx, NewLine            
;   call output    
   
   lea dx, SecondTerm            
   call output  
   

   lea di, inB
   call input  
   
   mov ax, N
   sub ax, cx     
   cmp ax, 5 ;check the input whether it is letter or digit
   JE CALC_SUM   
   mov cx, ax 
   lea si, inB;
   add si, ax
   sub si, 1
   lea di, inB + N-1
   call calc_shift
CALC_SUM:     
            
 ;  lea dx, NewLine            
 ;  call output   
   
;   lea si, inB 
;   call output_array     

   call calc_sum_proc  
       
   lea dx, NewLine            
   call output      
      
   lea si, inB 
  call output_array   
 
 ret   
 
   
              
output PROC          
    mov  ah, 09h             
    int  21h      
    ret                            
output ENDP          



output_array PROC   
     mov cx, N     
    start_array:
        mov  al, [si]        ;Element of a array
        add al,30h
        mov ah, 2   
        mov dl, al 
        int 21h     
        inc  si
    loop start_array
    ret                                
output_array ENDP 


 
 
input PROC      
    
    mov cx, N   
    cld                       


  L:
    mov ah, 0 
    int 16h   ;input symbol      
    
    cmp al,0Dh   ;check is the pressed key is 'ENTER' or not
    je RETURN     ;If it is equal to 'Enter' then go to exit
    cmp al, 39h ;check the input whether it is letter or digit
    JG L   
        
    cmp al, 30h ;check the input whether it is letter or digit
    JL L   
     
    mov ah, 2   
    mov dl, al 
    int 21h    
    sub al,30h ; get real value
    
    stosb     
    loop L
RETURN:                  
    ret            
input ENDP  
   
   

calc_sum_proc PROC   
      
   mov cx, N  
   lea si, inA + N-1   
   lea di, inB + N-1        
   mov dl,0 
next_digit: 
    mov  al, [si]  
    mov  bl, [di]   
    add al, bl  
    add al, dl     
    mov dl,0  
        
    cmp al,0xA 
    JL  NEXT     
    mov dl, 1 
    sub al,  0xA

NEXT:   
   mov [di], al 
   dec   si 
   dec   di
loop  next_digit
   ret             
  
    
calc_sum_proc ENDP      



calc_shift PROC     
    
 shift:  
  mov  al, [si]  
  mov  [di], al
  mov  [si], 0 
  dec si
  dec di
  loop shift
  ret 
calc_shift ENDP 

END main


;end start
