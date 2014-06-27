//-------------------
// MV
// version WIN32 et POCKETPC
// Sylvain Huet
// Derniere mise a jour : 07/01/2003
//

#include <stdio.h>
#include <string.h>

#include "param.h"
#include "terminal.h"
#include "memory.h"
#include "parser.h"
#include "prodbuffer.h"
#include "compiler.h"
#include "interpreter.h"


int Compiler::parseval()
{
    int k;

    k=parseval3();
    if (k)
    {
        return k;
    }

    if (!parser->next(0))
    {
        return 0;
    }
    if (strcmp(parser->token,"::"))
    {
        parser->giveback();
        return 0;
    }
    k=parseval();	// r�cursion
    if (k)
    {
        return k;    // r�cursion
    }

    k=createnodetype(TYPENAME_LIST);
    if (k)
    {
        return k;
    }
    k=createnodetype(TYPENAME_UNDEF);	// noeud �lement
    if (k)
    {
        return k;    // noeud �lement
    }
    TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));	// attachement du noeud �l�ment au noeud list
    k=unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(STACKGET(m,4)));	// unification �lement
    if (k)
    {
        return k;    // unification �lement
    }
    k=unif(VALTOPNT(STACKGET(m,1)),VALTOPNT(STACKGET(m,2)));	// unification list
    if (k)
    {
        return k;    // unification list
    }
    STACKSET(m,4,STACKGET(m,1));	// remplacement du type

    int* p=MALLOCCLEAR(m,LIST_LENGTH);	// cr�ation du tuple liste
    if (!p)
    {
        return MTLERR_OM;
    }
    TABSET(m,p,LIST_VAL,STACKGET(m,5));
    TABSET(m,p,LIST_NEXT,STACKGET(m,3));
    STACKSET(m,5,PNTTOVAL(p));
    STACKDROPN(m,4);
    return 0;
}


int Compiler::parseval3()
{
    int k,op,typ;

    k=parseval4();
    if (k)
    {
        return k;
    }
    while(1)
    {
        if (!parser->next(0))
        {
            return 0;
        }
        if (!strcmp(parser->token,"+"))
        {
            op=OPadd;
            typ=1;
        }
        else if (!strcmp(parser->token,"-"))
        {
            op=OPsub;
            typ=1;
        }
        else
        {
            parser->giveback();
            return 0;
        }
        k=parseval4();
        if (k)
        {
            return k;
        }
        if (op==OPadd)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))+VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPsub)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))-VALTOINT(STACKGET(m,1))));
        }
        int* ptyp;
        if (typ==1)
        {
            ptyp=VALTOPNT(TABGET(stdtypes,STDTYPE_I));
        }
        k=unif(VALTOPNT(STACKGET(m,0)),ptyp);
        if (k)
        {
            return k;
        }
        k=unif(VALTOPNT(STACKGET(m,2)),ptyp);
        if (k)
        {
            return k;
        }
        STACKDROPN(m,2);
    }
}

int Compiler::parseval4()
{
    int k,op,typ;

    k=parseval5();
    if (k)
    {
        return k;
    }
    while(1)
    {
        if (!parser->next(0))
        {
            return 0;
        }
        if (!strcmp(parser->token,"*"))
        {
            op=OPmul;
            typ=1;
        }
        else if (!strcmp(parser->token,"/"))
        {
            op=OPdiv;
            typ=1;
        }
        else if (!strcmp(parser->token,"%"))
        {
            op=OPmod;
            typ=1;
        }
        else
        {
            parser->giveback();
            return 0;
        }
        k=parseval5();
        if (k)
        {
            return k;
        }

        if (op==OPmul)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))*VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPdiv)
        {
            if (VALTOINT(STACKGET(m,1))==0)
            {
                return MTLERR_DIV;
            }
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))/VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPmod)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))%VALTOINT(STACKGET(m,1))));
        }
        int* ptyp;
        if (typ==1)
        {
            ptyp=VALTOPNT(TABGET(stdtypes,STDTYPE_I));
        }
        k=unif(VALTOPNT(STACKGET(m,0)),ptyp);
        if (k)
        {
            return k;
        }
        k=unif(VALTOPNT(STACKGET(m,2)),ptyp);
        if (k)
        {
            return k;
        }
        STACKDROPN(m,2);
    }
}

int Compiler::parseval5()
{
    int k,op;

    k=parseval6();
    if (k)
    {
        return k;
    }

    while(1)
    {
        if (!parser->next(0))
        {
            return 0;
        }
        if (!strcmp(parser->token,"&"))
        {
            op=OPand;
        }
        else if (!strcmp(parser->token,"|"))
        {
            op=OPor;
        }
        else if (!strcmp(parser->token,"^"))
        {
            op=OPeor;
        }
        else if (!strcmp(parser->token,"<<"))
        {
            op=OPshl;
        }
        else if (!strcmp(parser->token,">>"))
        {
            op=OPshr;
        }
        else
        {
            parser->giveback();
            return 0;
        }
        k=parseval6();
        if (k)
        {
            return k;
        }

        if (op==OPand)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))&VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPor)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))|VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPeor)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))^VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPshl)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))<<VALTOINT(STACKGET(m,1))));
        }
        else if (op==OPshr)
        {
            STACKSET(m,3,INTTOVAL(VALTOINT(STACKGET(m,3))>>VALTOINT(STACKGET(m,1))));
        }

        int* ptyp=VALTOPNT(TABGET(stdtypes,STDTYPE_I));
        k=unif(VALTOPNT(STACKGET(m,0)),ptyp);
        if (k)
        {
            return k;
        }
        k=unif(VALTOPNT(STACKGET(m,2)),ptyp);
        if (k)
        {
            return k;
        }
        STACKDROPN(m,2);
    }
}

int Compiler::parseval6()
{
    int k;

    if (!parser->next(0))
    {
        return parseval7();
    }

    if (!strcmp(parser->token,"-"))
    {
        if (!parser->next(0))
        {
            return parseval6();
        }
        if (isdecimal(parser->token))	// gestion des entiers
        {
            int i=-mtl_atoi(parser->token);
            k=STACKPUSH(m,INTTOVAL(i));
            if (k)
            {
                return k;
            }
            return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
        }
        parser->giveback();
        k=parseval6();
        if (k)
        {
            return k;
        }
        STACKSET(m,1,INTTOVAL(-VALTOINT(STACKGET(m,0))));
        return unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)));
    }
    else if (!strcmp(parser->token,"~"))
    {
        k=parseval6();
        if (k)
        {
            return k;
        }
        STACKSET(m,1,INTTOVAL(~VALTOINT(STACKGET(m,0))));
        return unif(VALTOPNT(STACKGET(m,0)),VALTOPNT(TABGET(stdtypes,STDTYPE_I)));
    }
    parser->giveback();
    return parseval7();
}


int Compiler::parseval7()
{
    int k;

    if (!parser->next(0))
    {
        PRINTF(m)(LOG_COMPILER,"Compiler : term expected (found EOF)\n");
        return MTLERR_SN;
    }
    if (!strcmp(parser->token,"("))	// gestion des parenth�ses
    {
        k=parseval();
        if (k)
        {
            return k;
        }
        return parser->parsekeyword(")");
    }
    else if (!strcmp(parser->token,"["))	// gestion des tuples
    {
        int nval=0;
        k=STACKPUSH(m,NIL);	// valeur finale
        if (k)
        {
            return k;    // valeur finale
        }
        k=STACKPUSH(m,NIL);	// type final
        if (k)
        {
            return k;    // type final
        }
        int sref=STACKREF(m);
        while(1)
        {
            if (!parser->next(0))
            {
                PRINTF(m)(LOG_COMPILER,"Compiler : ']' expected (found EOF)\n");
                return MTLERR_SN;
            }
            if (!strcmp(parser->token,"]"))
            {
                int* p=MALLOCCLEAR(m,nval);	// cr�ation du tuple liste
                if (!p)
                {
                    return MTLERR_OM;
                }
                int i;
                for(i=0; i<nval; i++)
                {
                    TABSET(m,p,i,STACKGET(m,(nval-i)*2-1));
                }
                STACKSETFROMREF(m,sref,1,PNTTOVAL(p));
                k=createnodetupleval(nval);
                if (k)
                {
                    return k;
                }
                STACKSETFROMREF(m,sref,0,STACKPULL(m));
                STACKRESTOREREF(m,sref);
                return 0;
            }
            parser->giveback();
            k=parseval();
            if (k)
            {
                return k;
            }
            nval++;
        }
    }
    else if (!strcmp(parser->token,"{"))	// gestion des tableaux
    {
        int nval=0;
        k=STACKPUSH(m,NIL);	// valeur finale
        if (k)
        {
            return k;    // valeur finale
        }
        k=createnodetype(TYPENAME_TAB);	// type final
        if (k)
        {
            return k;    // type final
        }
        k=createnodetype(TYPENAME_UNDEF);
        if (k)
        {
            return k;
        }
        TABSET(m,VALTOPNT(STACKGET(m,1)),TYPEHEADER_LENGTH,STACKGET(m,0));
        int* p=VALTOPNT(STACKPULL(m));

        while(1)
        {
            if (!parser->next(0))
            {
                PRINTF(m)(LOG_COMPILER,"Compiler : '}' expected (found EOF)\n");
                return MTLERR_SN;
            }
            if (!strcmp(parser->token,"}"))
            {

                k=DEFTAB(m,nval);
                if (k)
                {
                    return k;
                }

                STACKSET(m,2,STACKGET(m,0));	// report de la valeur du tableau
                STACKDROP(m);
                return 0;
            }
            parser->giveback();
            k=parseval();
            if (k)
            {
                return k;
            }
            k=unif(VALTOPNT(STACKGET(m,0)),p);
            if (k)
            {
                return k;
            }
            STACKDROP(m);
            nval++;
        }
    }
    else if (!strcmp(parser->token,"nil"))	// gestion du nil
    {
        k=STACKPUSH(m,NIL);	// valeur
        if (k)
        {
            return k;    // valeur
        }
        return createnodetype(TYPENAME_WEAK);
    }
    else if (!strcmp(parser->token,"'"))	// gestion des 'char
    {
        if (!parser->next(0))
        {
            PRINTF(m)(LOG_COMPILER,"Compiler : 'char expected (found EOF)\n");
            return MTLERR_SN;
        }
        k=STACKPUSH(m,INTTOVAL(parser->token[0]&255));	// valeur
        if (k)
        {
            return k;    // valeur
        }
        k=parser->parsekeyword("'");
        if (k)
        {
            return k;
        }
        return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
    }
    else if (isdecimal(parser->token))	// gestion des entiers
    {
        int i=mtl_atoi(parser->token);
        k=STACKPUSH(m,INTTOVAL(i));	// valeur
        if (k)
        {
            return k;    // valeur
        }
        return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
    }
    else if ((parser->token[0]=='0')&&(parser->token[1]=='x')
             &&(ishexadecimal(parser->token+2)))	// gestion des entiers
    {
        int i=mtl_htoi(parser->token+2);
        k=STACKPUSH(m,INTTOVAL(i));	// valeur
        if (k)
        {
            return k;    // valeur
        }
        return STACKPUSH(m,TABGET(stdtypes,STDTYPE_I));
    }
    else if (parser->token[0]=='"')	// gestion des chaines
    {
        k=parser->getstring(m,'"');
        if (k)
        {
            return k;
        }
        return STACKPUSH(m,TABGET(stdtypes,STDTYPE_S));
    }
    else
    {
        PRINTF(m)(LOG_COMPILER,"Compiler : unexpected term '%s'\n",parser->token);
        return MTLERR_SN;
    }
}
