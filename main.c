/**
PROGRAM : PASS 1 ASSEMBLER FOR MOTOROLA 68000
INPUT : ALP BASED ON M68000 INSTRUCTION SET
OUTPUT : INTERMEDIATE FILE,SYMTAB,PROGRAM LENGTH
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int digit_to_int(char);
/// ///////////////////////
typedef struct code
{
    char label[50];
    char mnemonic[10];
    char operator1[50];
    struct code *next;
};
/// ///////////////////////
typedef struct code *node;
node func1()
{
    return malloc(sizeof(struct code));     ///returns address
}
node arr[20];
int startaddr;
int errorFlag=0;
int progSize=0;
char stmt3[100]="\0";
void assm()
{
    FILE *fp1,*fp2,*fp3,*fp4;
    char len[10];
    char *token1;
    int i=0;
    fp1=fopen("source2.txt","r");
    while(!feof(fp1))
    {
        arr[i]=func1();
        fscanf(fp1,"%s  %s  %s",arr[i]->label,arr[i]->mnemonic,arr[i]->operator1); /// reading each line
        if(strcmpi(arr[i]->mnemonic,"END")==0)
        {
            break;
        }
        i++;
    }
    fclose(fp1);
    if(strcmpi(arr[0]->mnemonic,"ORG")==0)
    {
        if(strstr(arr[0]->operator1,"$"))
        {
            token1=strtok(arr[0]->operator1,"$");
            startaddr=atoi(token1);
        }
        else
        {
            startaddr=0;
        }
    }
    process(i);
}
char* get_opcode(char mnemonic[],char ch[],int i)
{
    ///mnemonic   operands  opcode  size
    FILE *fp1;
    char opcode[8]="\0";
    int length_of_instr=-1;
    char line[80], field1[32], field2[32], field3[32],field4[32];
    fp1=fopen("OPTAB2.txt","r");
    while (fgets(line, sizeof(line), fp1) != NULL)
    {
        if(i==0)
        {
            sscanf(line, "%s\t%s\t%s", field1, field2, field3);
            if(strcmpi(mnemonic,field1)==0)
            {
                length_of_instr=atoi(field4);
                strcpy(opcode,field3);

            }
        }
        sscanf(line, "%s\t%s\t%s\t%s", field1, field2, field3, field4);
        if(((strcmpi(mnemonic,field1))==0) && ((strcmpi(ch,field2))==0))
        {
            length_of_instr=atoi(field4);
            strcpy(opcode,field3);
        }
    }
    fclose(fp1);
    if(length_of_instr==-1)
    {
        errorFlag=1;
        ///return;
    }
    return opcode;
}
/// /////////////////////////////////////////////////
char* search_symtab(char temp[],FILE *fp)
{
    char tmp[10],line[80];
    char line_no[5],addr[17]="\0",initialaddr[17];
    int i=0;
    fp=fopen("symtab.txt","r+");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        sscanf(line, "%s\t%s\t%s",line_no,addr,tmp);
        printf("",temp);
        if(strcmpi(tmp,temp)==0)
        {
            return addr;
        }
    }
    return "\0";
}
/// ////////////////////////////////////////////////
void process(int end)
{
    char opcode[8];
    int i=0,x=0,len=0,opnum=0;
    char *token1,*token2,*ptr;
    char tempOp[100],newOp[100];
    static long address=0;
    int locctr[100];
    locctr[0]=0000;
    char *addr[100],temp2[30];
    FILE *fp1,*fp2;
    int isOpValid=0;
    int length_of_instr=0;
    fp1=fopen("inter.txt","w");
    fp2=fopen("symTab.txt","w");
    while(i<=end)
    {
        /// /////////// NUMBER OF OPERANDS /////////////////
        int y=0;
        opnum=0;
        if(strcmpi(arr[i]->operator1,"-")!=0)
        {
            strcpy(newOp,arr[i]->operator1);
            for(y=0;newOp[y]!='\0';y++)
            {
                if(newOp[y]==',')
                {
                    opnum++;
                }
            }
            opnum++;
        }
        ///printf("OPERAND COUNT : %d\n",opnum);
        /// ////////////////////////////////////////////////
        strcpy(tempOp,arr[i]->operator1);
        if(strcmpi(arr[i]->label,"-")==0)
        {
            if(strcmpi(arr[i]->mnemonic,"ORG")==0)
            {
                x++;
                length_of_instr=0;
                strcpy(temp2,arr[i]->operator1);
                len=strlen(temp2);
                if(strstr(arr[i]->operator1,"$"))
                {
                    fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                    token2=strtok(arr[i]->operator1,"$");
                    address = strtoul(token2, &ptr, 16); ///converting to hex
                }
                else
                {
                    fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                    token2=strtok(arr[i]->operator1,"$");
                    address=atoi(token2);
                    ///fprintf(fp1,"%.2d\t%d\t%.4x\t%s\t%s\t\t%s\n",(i+1),length_of_instr,address,arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                }
            }
            else
            {
                if(strcmpi(arr[i]->mnemonic,"END")==0)
                {
                    length_of_instr=0;
                    strcpy(opcode,"-");
                }
                else
                {
                    token(tempOp);  ///get type of operands
                    length_of_instr=search_optab(arr[i]->mnemonic,stmt3,opnum);     ///get length of instruction
                    /****************OPCODE**********************/
                    strcpy(opcode,get_opcode(arr[i]->mnemonic,stmt3,opnum));    ///get opcode from OPTAB
                    if(!strcmpi(opcode,"\0"))
                    {
                        printf("Invalid Mnemonic '%s' at line %d\n",arr[i]->mnemonic,(i+1));    ///display if wrong mnemonic
                        fprintf(fp1," ERROR : Invalid opcode at line %d\n",(i+1));
                    }
                    /********************************************/
                    progSize=progSize+length_of_instr;
                }
                fprintf(fp1,"%.2d\t%s\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),opcode,length_of_instr,address,
                        arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                address+=length_of_instr;
            }
        }
        else
        {

                if(strcmpi(arr[i]->mnemonic,"DC.B")==0)
                {
                    int x=0,numCount=0;
                    char operands[100];
                    strcpy(operands,arr[i]->operator1);
                    if(operands[0]=='\'')   ///if the data is string
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            numCount++;
                        }
                        length_of_instr=numCount-2;
                    }
                    else
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            if(operands[x]==',')
                            {
                                numCount++;
                            }
                        }
                        strcpy(opcode,"-");
                        length_of_instr=numCount+1;     ///size of each value is 1 bytes
                    }
                    fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,
                            arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                    search(arr[i]->label,fp2,address,i+1);      ///search for duplicate label
                    address+=length_of_instr;
                    progSize=progSize+length_of_instr;
                }
                else if(strcmpi(arr[i]->mnemonic,"DC.W")==0)
                {
                    int x=0,numCount=0;
                    char operands[100];
                    strcpy(operands,arr[i]->operator1);
                    if(operands[0]=='\'')
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            numCount++;
                        }
                        length_of_instr=numCount-2;
                    }
                    else
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            if(operands[x]==',')
                            {
                                numCount++;
                            }
                        }
                        strcpy(opcode,"-");
                        length_of_instr=(numCount+1)*2;     ///size of each value is 2 bytes
                        }
                        fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,
                            arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                        search(arr[i]->label,fp2,address,i+1);      ///search for duplicate label
                        address+=length_of_instr;
                        progSize=progSize+length_of_instr;
                }
                else if(strcmpi(arr[i]->mnemonic,"DC.L")==0)
                {
                    int x=0,numCount=0;
                    char operands[100];
                    strcpy(operands,arr[i]->operator1);
                    if(operands[0]=='\'')
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            numCount++;
                        }
                        length_of_instr=numCount-2;
                    }
                    else
                    {
                        for(x=0;operands[x]!='\0';x++)
                        {
                            if(operands[x]==',')
                            {
                                numCount++;
                            }
                        }
                        strcpy(opcode,"-");
                        length_of_instr=(numCount+1)*4;     ///size of each value is 4 bytes
                    }
                    fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,
                            arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                    search(arr[i]->label,fp2,address,i+1);      ///search for duplicate label
                    address+=length_of_instr;
                    progSize=progSize+length_of_instr;
                }
                else if(strcmpi(arr[i]->mnemonic,"EQU")==0)
                {
                    long value=0;
                    char *addr;
                    length_of_instr=0;
                    addr=search_symtab(arr[i]->operator1,fp2);  ///returns address of label if found
                    if(strcmp(addr,"\0")!=0)
                    {
                        value = strtoul(addr, &ptr, 16);
                        fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,
                        arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                        search(arr[i]->label,fp2,value,i+1);    ///search for duplicate label

                    }
                    else
                    {
                        value=atoi(arr[i]->operator1);
                        fprintf(fp1,"%.2d\t-\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),length_of_instr,address,
                        arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                        search(arr[i]->label,fp2,value,i+1);    ///search for duplicate label
                    }

                }
            else
            {
                token(tempOp);      ///get type of operands
                length_of_instr=search_optab(arr[i]->mnemonic,stmt3,opnum);     ///get length of instruction
                /****************OPCODE**********************/
                strcpy(opcode,get_opcode(arr[i]->mnemonic,stmt3,opnum));
                if(!strcmpi(opcode,"\0"))
                {
                    printf("Invalid Mnemonic '%s' at line %d\n",arr[i]->mnemonic,(i+1));    ///display if wrong mnemonic
                    fprintf(fp1," ERROR : Invalid opcode at line %d\n",(i+1));
                }
                /********************************************/
                progSize=progSize+length_of_instr;
                fprintf(fp1,"%.2d\t%s\t%d\t%.4x\t%s\t%s\t%s\n",(i+1),opcode,length_of_instr,address,
                            arr[i]->label,arr[i]->mnemonic,arr[i]->operator1);
                search(arr[i]->label,fp2,address,i+1);
                address+=length_of_instr;
            }
        }
        i++;
    }
    fclose(fp1);
    fclose(fp2);
}
void search(char temp[],FILE *fp,int loc,int line_num)
{
    char tmp[10],line[80];
    char line_no[5],addr[17];
    int i=0;
    fp=fopen("symtab.txt","r+");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        sscanf(line, "%s%s%s",line_no,addr,tmp);
        if(strcmpi(tmp,temp)==0)
        {
             printf("\n*Error in line: %d Duplicate symbol Found : %s\n",line_num,temp);
             errorFlag=1;
             return;
        }
    }
    if(fp!=NULL )
    {
        fprintf(fp,"%d\t%.4x\t%s\n",line_num,loc,temp);     ///prints to SYMTAB if label not found
        fclose(fp);
    }
}
/**int retHex(char temp[])
{
    int i=0,num=0,r=0,dnum=0;
    num=atoi(temp);
    while(num>0)
    {
        r=num%10;
        dnum=dnum+(r*pow(16,i));
        num=num/10;
        i++;
    }
    return dnum;
}**/
int search_optab(char mnemonic[],char ch[],int i)
{
    ///OPTAB format :   mnemonic   operands   opcode     size
    FILE *fp1;
    int length_of_instr=-1;
    char line[80], field1[32], field2[32], field3[32],field4[32];
    fp1=fopen("OPTAB2.txt","r");
    while (fgets(line, sizeof(line), fp1) != NULL)
    {
        if(i==0)
        {
            sscanf(line, "%s\t%s\t%s", field1, field2, field3);
            if(strcmpi(mnemonic,field1)==0)
            {
                length_of_instr=atoi(field4);
            }
        }
        sscanf(line, "%s\t%s\t%s\t%s", field1, field2, field3, field4);
        if(((strcmpi(mnemonic,field1))==0) && ((strcmpi(ch,field2))==0))
        {
            length_of_instr=atoi(field4);
        }
    }
    fclose(fp1);
    if(length_of_instr==-1)
    {
        errorFlag=1;
        ///printf("Invalid Opcode");
        return;
    }
    return length_of_instr;
}

char* multiple(char temp[10][200],int num)
{
    int i=0;
    char stmt2[100]="\0";
    for(i=0;i<num;i++)
    {
        if(strstr(temp[i],"#"))
        {
            strcpy(temp[i],"#data");    ///Immediate Addressing Mode
        }
        else
        {
            if((temp[i][0]=='d'||temp[i][0]=='D')||(temp[i][0]=='a'||temp[i][0]=='A'))      ///Data and Address Register Direct Addressing Mode
            {
                if(digit_to_int(temp[i][1])>7)
                {
                    errorFlag=1;
                    continue;
                }
                else
                {

                }
            }
            else if(temp[i][0]=='(')
            {
                if((temp[i][1]=='d'||temp[i][1]=='D')||(temp[i][1]=='a'||temp[i][1]=='A'))      ///Address Resister Indirect
                {
                    if(digit_to_int(temp[i][2])>7)
                    {
                        errorFlag=1;
                        continue;
                    }
                    else
                    {
                        ///strcpy(temp[0],"(addr)");
                    }
                }
                else
                    strcpy(temp[i],"(addr)");   ///Address Indirect
            }
            else
            {
                strcpy(temp[i],"addr");     ///Absolute Addressing Mode
            }
        }
    }
    for(i=0;i<num;i++)
    {
        if(i==0)
            goto LOOP;
        strcpy(stmt2,strcat(stmt2,","));
        LOOP:strcpy(stmt2,strcat(stmt2,temp[i]));   ///concatinating all operands
    }
    return stmt2;
}
void token(char stmt[100])
{
    char *token;
    char temp[10][200];
    token = strtok(stmt," :,");
    int j=0,i=0;
    while( token != NULL )
    {
        strcpy(temp[j],token);
        j++;
        token = strtok(NULL, " :,");
    }
    if(j==1)
    {
        if(strstr(temp[0],"#"))
        {
            strcpy(temp[0],"#data");   ///Immediate Addressing Mode
        }
        else
        {
            if((temp[0][0]=='d'||temp[0][0]=='D')||(temp[0][0]=='a'||temp[0][0]=='A') )  ///Data and Address Register Direct Addressing Mode
            {
                if(digit_to_int(temp[0][1])>7)
                {
                    printf("invalid opcode\n");
                    return;
                }

                else
                {

                }
            }
            else if(temp[0][0]=='(')
            {
                if((temp[0][1]=='d'||temp[0][1]=='D')||(temp[0][1]=='a'||temp[0][1]=='A'))  ///Address Resister Indirect
                {
                    if(digit_to_int(temp[0][2])>7)
                     {
                         printf("invalid opcode\n");
                         return;
                     }
                    else
                    {
                        ///strcpy(temp[0],"(addr)");
                    }
                }
                else
                    strcpy(temp[0],"(addr)");   ///Address Indirect
            }
            else
            {
                strcpy(temp[0],"addr");     ///Absolute Addressing Mode
            }
        }
        strcpy(stmt3,temp[0]);
    }
    else if(j>1)
    {
        strcpy(stmt3,multiple(temp,j));     ///Jumps if no. of operands > 1
    }
}
int digit_to_int(char d)
{
 char str[2];

 str[0] = d;
 str[1] = '\0';
 return (int) strtol(str, NULL, 10);
}

int main()
{
    FILE *fp;
    fp=fopen("length.h","w");
    int i=0,found=0;
    assm();
    if(errorFlag==1)
    {
        printf("\nASSEMBLING FAILED\n");
    }
    else
    {
        printf("No Errors Found..");
        printf("\nAssembling Complete\n");
    }
    printf("\nProgram Size is : '%xH' bytes\n\n",progSize);
    fprintf(fp,"#define length %d",progSize); /// Used by Pass 2
    return 0;
}
