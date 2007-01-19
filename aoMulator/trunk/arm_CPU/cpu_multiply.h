
void arm_Mul(int condCode,uint32_t instruction)
{
    if(!checkCondition(condCode))
    {
        DEBUG("CC not met\n");
        return;
    }
    
    int Rd,Rn,Rs,RdHi,RdLo,Rm;
    uint32_t val,valHi,valLo;
    bool bit_U,bit_A,bit_S,longM;

    Rm= instruction & 0xF;
    Rs= (instruction>>8) & 0xF;
    longM=(instruction>>23) & 0x1;
    bit_S=(instruction>>20) & 0x1;
    bit_A=(instruction>>21) & 0x1;

    if(!longM)                                 // multiply normal
    {
        Rd= (instruction>>16) & 0xF;
        Rn= (instruction>>12) & 0xF;
        if(bit_A)
        {
            REG(Rd) = (uint32_t)((((uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs))+(uint64_t)GET_REG(Rn)) & 0x00000000FFFFFFFF);
            DEBUG("MLA %s, %s, %s, %s\n",RR(Rd),RR(Rn),RR(Rm),RR(Rs));
        }
        else
        {
            REG(Rd) = (uint32_t)(((uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs)) & 0x00000000FFFFFFFF);
            DEBUG("MUL %s, %s, %s\n",RR(Rd),RR(Rm),RR(Rs));
        }
        
        if(bit_S)
        {
            ARM_NegZero(GET_REG(Rd));
        }
    }
    else                                       // multiply long
    {
        uint64_t value;
        
        bit_U=(instruction>>22) & 0x1;
        RdHi=(instruction>>16) &0xF;
        RdLo=(instruction>>12) &0xF;
        
        DEBUG("%sM%sL%s %s, %s, %s, %s\n",bit_U?"S":"U",bit_A?"LA":"UL",bit_S?"S":"",RR(RdLo),RR(RdHi),RR(Rm),RR(Rs));

        if (!bit_U)
        {
            value = (uint64_t)GET_REG(Rm) * (uint64_t)GET_REG(Rs);
            
            if(bit_A) // accumulate
            {
                value += (((uint64_t)REG(RdHi))<<32) | ((uint64_t)REG(RdLo));
            }
            
            REG(RdHi) = (uint32_t)(value >> 32);
            REG(RdLo) = (uint32_t)(value & 0x00000000FFFFFFFF);

            if(bit_S)
            {
                SET_N(((valHi >> 31) & 0x1) == 1);
                SET_Z((valHi == 0 && valLo == 0));
            }
            
        }
        else
        {
            int v1=GET_REG(Rm);
            int v2=GET_REG(Rs);
            int v3=0xFFFFFFFF;
            int s1=1,s2=1;
            int a,b,c,d,x,y;
            
            if(v1&0x80000000) {v1=~v1+1;s1=-1;}
            if(v2&0x80000000) {v2=~v2+1;s2=-1;}
            
            a=(v1>>16)&0xFFFF;
            b=v1&0xFFFF;
            c=(v2>>16)&0xFFFF;
            d=v2&0xFFFF;
            
            v1 = b * d;                   /* BD */
            x = a * d + c * b;            /* AD + CB */
            y = ((v1 >> 16) & 0xffff) + x;

            v1 = (v1 & 0xffff) | ((y & 0xffff) << 16);
            v2 = (y >> 16) & 0xffff;

            v2 += a * c;                  /* AC */
            
            if(s1*s2<0) { v1=(~v1+1)&0xFFFFFFFF; v2=~v2; if(v1==0) v2++;};
            
            value = 0xFFFFFFFF + 5;
            
            if(bit_A) // accumulate
            {
                v3=REG(RdLo);
                x=v1+v3;
                if ((isNeg(v1) && isNeg(v1)) ||
                     (isNeg(v1) && !isNeg(x)) ||
                     (isNeg(v2) && !isNeg(x)))
                    y=1;
                else
                    y=0;
                v1=x;
                v2=v2+REG(RdHi)+1;
                printf("AA\n");
            }
            
            REG(RdHi) = v2;
            REG(RdLo) = v1;
            
            if(bit_S)
            {
                SET_N(((v2 >> 31) & 0x1) == 1);
                SET_Z((v2 == 0 && v1 == 0));
            }
        }

        
        
        
        
    }
}
