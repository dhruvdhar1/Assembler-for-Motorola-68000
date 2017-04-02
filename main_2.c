#include <stdio.h>
#include <stdlib.h>
#define str_addr 0
#define length 52
struct command
{
    char line[10],location[10],label[100],mnemonic[100],variable[100][100],opcode[20],size[20];
};
int main()
{
    struct command com;
    char search[100]="A0A1A2A3A4A5A6A7(A0)(A1)(A2)(A3)(A4)(A5)(A6)(A7)D0D1D2D3D4D5D6D7(D0)(D1)(D2)(D3)(D4)(D5)(D6)(A7)a0a1a2a3a4a5a6a7(a0)(a1)(a2)(a3)(a4)(a5)(a6)(a7)d0d1d2d3d4d5d6d7(d0)(d1)(d2)(d3)(d4)(d5)(d6)(d7)";
    FILE *f_op,*f_inter,*f_obj;
    char var[100],*tok,var_cpy[100],op_code[100]="",*ptr,txt[100]="",op[31],*addr_ptr,addr[20],loc[8]="",start[8]="",*ptrr;
    unsigned long opcode;
    addr_ptr=&addr;
    int i=0,count=0,size=0,j=0,len=0,full_len=0,flag=0;
    f_op=fopen("object_prog.txt","w");
    f_inter=fopen("inter.txt","r");
    fprintf(f_op,"H ^ PROG ^ %.6x ^ %.6x",str_addr,length);
    f_obj=fopen("obj.txt","w");
    close(f_obj);

    while(1)
    {
        len=0;
        fscanf(f_inter,"%s",com.line);
        fscanf(f_inter,"%s",com.opcode);
        fscanf(f_inter,"%s",com.size);
        fscanf(f_inter,"%s",com.location);
        fscanf(f_inter,"%s",com.label);
        fscanf(f_inter,"%s",com.mnemonic);
        fscanf(f_inter,"%s",var);
        strcpy(op,com.opcode);
        strcpy(var_cpy,var);
        if(flag==0){
            strcpy(txt,"");
            strcpy(loc,"00");
            strcpy(loc,strcat(loc,com.location));
            flag=1;
            strcpy(start,loc);
        }
        //printf("*%s*\n",com.mnemonic);
        //printf("\n%s\t%s\t%s\t",com.line,com.location,com.mnemonic);
        if(!strcmpi(com.mnemonic,"end"))
        {
            break;
        }
        ///Tokanization///
        tok=strtok(var,",");
        for(i=0;;i++)
        {
            if(tok==NULL)
                break;
            strcpy(com.variable[i],tok);
            tok = strtok(NULL,",");
        }
        ///*///////////////
        strcpy(com.variable[i],"\0");
        if(!strcmpi(com.mnemonic,"dc"))
        {
            f_obj=fopen("obj.txt","r+");

            if(var_cpy[0]=='"')
            {
                for(i=0;var_cpy[i]!='\0';i++)
                {
                    count++;
                }
                for(i=0;var_cpy[i]!='\0';i++)
                {
                    if(i==0 || i==count-1)
                        continue;
                    else
                        fprintf(f_obj,"%2.2d",var_cpy[i]);
                }
            }
            else
            {
                for(i=0;;i++)
                {
                    unsigned long x=0;
                    if(!strcmp(com.variable[i],"\0"))
                        break;
                    if(com.variable[i][0]=='$')
                    {
                        com.variable[i][0]='0';
                        x=strtoul(com.variable[i],&ptrr,16);
                        fprintf(f_obj,"%2.4x",x);
                    }
                    else
                    {
                        for(j=0;com.variable[i][j]!='\0';j++)
                        {
                            int y=com.variable[i][j];
                            x+=(y-48)*(pow(10,strlen(com.variable[i])-j-1)); ///converts ascii to decimal
                            ///printf("%d\n",x);
                        }
                        fprintf(f_obj,"%2.4x",x);
                    }
                    count++;
                }
            }
            fprintf(f_obj,"\n");
            fclose(f_obj);
            f_obj=fopen("obj.txt","r");
            fscanf(f_obj,"%s",op);
            fclose(f_obj);
            //printf("$$$%s$$$",op);
            len=strlen(op)/2;
        }
        else if(!strcmpi(com.mnemonic,"org"))
        {
            ///Do_Nothing
        }
        else
        {
            for(i=0;;i++)
            {
                if(!strcmp(com.variable[i],"\0"))
                    break;
                if(com.variable[i][0]=='#' && com.variable[i][1]=='$')
                {
                    char x[10];
                    ///com.variable[i][1]='';
                    for(j=2;com.variable[i][j]!='\0';j++)
                        x[j-2]=com.variable[i][j];
                    x[j-1]='\0';
                    strcpy(op,strcat(op,x));
                }
                /**DECIMAL DATA
                if(com.variable[i][0]=='#' && com.variable[i][1]=='$')
                {
                    char x[10];
                    for(j=1;com.variable[i][j]!='\0';j++)
                        x[j-1]=com.variable[i][j];
                    x[j-1]='\0';
                    strcpy(op,strcat(op,x));
                }
                **/
                else if(!(strstr(search,com.variable[i]) != NULL) && com.variable[i][0]!='-')
                {
                    find(com.variable[i],&addr);///**
                    if(addr[0]=='\0')
                    {
                        printf("line:%s   '%s' symbol not found\n",com.line,com.variable[i]);
                        if((!strcmpi(com.mnemonic,"jmp"))||(!strcmpi(com.mnemonic,"jsr"))
                           ||(!strcmpi(com.mnemonic,"bra.w")))
                            strcpy(op,strcat(op,"0000"));
                        else
                            strcpy(op,strcat(op,"00"));
                    }
                    else
                    {
                        //strtoul(addr,&ptrr,16)-strtoul(com.location,&ptrr,16)
                        if((!strcmpi(com.mnemonic,"jmp"))||(!strcmpi(com.mnemonic,"jsr"))
                           ||(!strcmpi(com.mnemonic,"bra.w")));
                        else
                        {
                            unsigned long a=strtoul(addr,&ptrr,16) -
                                (strtoul(com.location,&ptrr,16)+strtoul(com.size,&ptrr,16)); ///Displacement calc
                            a=a%256;
                            //char x[50]="";
                            FILE *f_ptr;
                            //strcpy(x,printf("%2.2x",a));
                            f_ptr=fopen("xxx.txt","w");
                            fprintf(f_ptr,"%2.2x",a);
                            fclose(f_ptr);
                            f_ptr=fopen("xxx.txt","r");
                            fscanf(f_ptr,"%s",addr);
                            fclose(f_ptr);
                        }
                        strcpy(op,strcat(op,addr));
                    }
                }
                len=strlen(op)/2;
            }
        }
        if(flag==1)
            flag++;
        else if((full_len+len)<31 && strcmpi(com.mnemonic,"org"))
        {
            strcpy(txt,strcat(txt," ^ "));
            strcpy(txt,strcat(txt,op));
            full_len+=len;
            //strcpy(op_code,strcat(s,op_code));
            //strcpy(txt,strcat(txt,op_code));
            //printf("\n*%d",full_len);
        }
        else{
            //printf("\n!%d",full_len);
            fprintf(f_op,"\nT ^ %s ^ %.6x%s ",loc,full_len,txt);
            //printf("ZZZZZZZZZZ%sZZ\n",loc);
            full_len=len;
            strcpy(txt,"");
            strcpy(loc,"00");
            strcpy(loc,strcat(loc,com.location));
        }


        strcpy(op,"");
    }
    fprintf(f_op,"\nT ^ %s ^ %.6x%s ",loc,full_len,txt);
    fprintf(f_op,"\nE ^ %s",start);
    fclose(f_obj);
    return 0;
}

void find(char str[100], char *addr_ptr)
{
    FILE *f_symtab;
    char sym[100],addr[20];
    int i=0;
    f_symtab=fopen("symtab.txt","r");
    while(1)
    {
        fscanf(f_symtab,"%s",addr);
        fscanf(f_symtab,"%s",addr);
        fscanf(f_symtab,"%s",sym);
        if(feof(f_symtab))
            break;
        if(!strcmp(sym,str))
        {
            strcpy(addr_ptr,addr);
            return;
        }
    }
    strcpy(addr_ptr,"");
}
