#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEYWORDS 7
#define ID_LENGTH 12

enum tsymbol
{
    tnull = -1,
    tnot,
    tnotequ,
    tmod,
    tmodAssign,
    tident, //Variable
    tnumber,
    tand,
    tlparen,
    trparen,
    tmul,
    tmulAssign,
    tplus,
    tinc,
    taddAssign,
    tcomma,
    tminus,
    tdec,
    tsubAssign,
    tdiv,
    tdivAssign,
    tsemicolon,
    tless,
    tlesse,
    tassign,
    tequal,
    tgreat,
    tgreate,
    tlbracket,
    trbracket,
    teof,
    tconst,
    telse,
    tif,
    tint,
    treturn,
    tvoid,
    twhile,
    tlbrace, //{
    tor,
    trbrace //}

};

struct tokenType
{
    int number; // token number
    char sym[2];
    union {
        char id[ID_LENGTH];
        int num;
    } value; //token value
} token;

char *keyword[NO_KEYWORDS] = {
    "const", "else", "if", "int", "return", "void", "while"};

enum tsymbol tnum[NO_KEYWORDS] = {
    tconst, telse, tif, tint, treturn, tvoid, twhile};
int superLetter(char ch)
{
    if (isalpha(ch) || ch == '_')
        return 1;
    else
        return 0;
}

int superLetterOrDigit(char ch)
{
    if (isalnum(ch) || ch == '_')
        return 1;
    else
        return 0;
}

int hexValue(char ch)
{
    switch (ch)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return (ch - '0');
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return (ch - 'A' + 10);
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return (ch - 'a' + 10);
    default:
        return -1;
    }
}

int getIntNum(char firstCharacter)
{
    int num = 0;
    int value;
    char ch;

    if (firstCharacter != '0')
    {
        ch = firstCharacter;

        do
        {
            num = 10 * num + (int)(ch - '0'); // 10����
            ch = getchar();
        } while (isdigit(ch));
    }
    else
    {
        ch = getchar();
        if ((ch >= '0') && (ch <= '7'))
            do
            {
                num = 8 * num + (int)(ch - '0');
                ch = getchar();
            } while ((ch <= '0') && (ch <= '7'));

        else if ((ch == 'X') || (ch == 'x'))
        {
            while ((value = hexValue(ch = getchar())) != -1)
                num = 16 * num + value;
        }

        else
            num = 0;
    }
    ungetc(ch, stdin);
    return num;
}

void lexicalError(int n)
{
    printf(" *** Lexical Error : ");
    switch (n)
    {
    case 1:
        printf("an identifier length must be less than 12.\n");
        break;
    case 2:
        printf("next character must be &.\n");
        break;
    case 3:
        printf("next character must be |. \n");
        break;
    case 4:
        printf("invalid character!!!\n");
        break;
    }
}

struct tokenType scanner()
{
    struct tokenType token;
    int i, index;
    char ch, id[ID_LENGTH];

    token.number = tnull;

    do
    {
        while (isspace(ch = getchar()))
            ; // state 1 : skip blanks

        if (superLetter(ch)) // identifier or keyword
        {
            i = 0;
            do
            {
                if (i < ID_LENGTH)
                    id[i++] = ch;
                ch = getchar();
            } while (superLetterOrDigit(ch));
            if (i >= ID_LENGTH)
                lexicalError(1);
            id[i] = '\0';
            ungetc(ch, stdin); //retract
            // find the identifier in the keyword table
            for (index = 0; index < NO_KEYWORDS; index++)
                if (!strcmp(id, keyword[index]))
                    break;
            if (index < NO_KEYWORDS) // found, keyword exit
            {
                token.number = tnum[index];
                strcpy(token.value.id, id);
            }

            else // not found, identifier exit
            {
                token.number = tident;
                strcpy(token.value.id, id);
            }

        } //end of identifier or keyword

        else if (isdigit(ch))
        {
            //integer constant
            token.number = tnumber;
            token.value.num = getIntNum(ch);
        }

        else
            switch (ch)
            {
            //special character
            case '/': //stage10
                ch = getchar();
                if (ch == '*') //text comment
                {
                    strcpy(token.sym, "/*");
                    do
                    {
                        while (ch != '*')
                            ch = getchar();
                        ch = getchar();
                    } while (ch != '/');
                    strcpy(token.sym, "*/");
                }
                else if (ch == '/') //line comment
                {
                    strcpy(token.sym, "//");
                    while (getchar() != '\n')
                        ;
                }

                else if (ch == '=')
                {
                    strcpy(token.sym, "/=");
                    token.number = tdivAssign;
                }

                else
                {
                    token.number = tdiv;
                    ungetc(ch, stdin); // retract
                }

                break;

            case '!': //state 17
                ch = getchar();
                if (ch == '=')
                {
                    strcpy(token.sym, "!=");
                    token.number = tnotequ;
                }

                else
                {
                    strcpy(token.sym, "!");
                    token.number = tdiv;
                    ungetc(ch, stdin); // retract
                }
                break;

            case '%': //state 20
                ch = getchar();
                if (ch == '=')
                {
                    strcpy(token.sym, "%=");
                    token.number = tmodAssign;
                }

                else
                {
                    strcpy(token.sym, "%");
                    token.number = tmod;
                    ungetc(ch, stdin);
                }
                break;

            case '&': //state 23
                ch = getchar();
                if (ch == '&')
                {
                    strcpy(token.sym, "&&");
                    token.number = tand;
                }

                else
                {
                    lexicalError(2);
                    ungetc(ch, stdin);
                }

                break;

            case '*': //state 25
                ch = getchar();
                if (ch == '=')
                {
                    strcpy(token.sym, "*=");
                    token.number = tmulAssign;
                }

                else
                {
                    strcpy(token.sym, "*");
                    token.number = tmul;
                    ungetc(ch, stdin); // retract
                }
                break;

            case '+': //state 28
                ch = getchar();
                if (ch == '+')
                {
                    strcpy(token.sym, "++");
                    token.number = tinc;
                }

                else if (ch == '=')
                {
                    strcpy(token.sym, "+=");
                    token.number = taddAssign;
                }

                else
                {
                    strcpy(token.sym, "+");
                    token.number = tplus;
                    ungetc(ch, stdin); // retract
                }

                break;

            case '-': //state 32
                ch = getchar();
                if (ch == '-')
                {
                    strcpy(token.sym, "--");
                    token.number = tdec;
                }

                else if (ch == '=')
                {
                    strcpy(token.sym, "-=");
                    token.number = tsubAssign;
                }

                else
                {
                    strcpy(token.sym, "-");
                    token.number = tminus;
                    ungetc(ch, stdin); //retract
                }

                break;

            case '<': //state 36
                ch = getchar();
                if (ch == '=')
                {
                    strcpy(token.sym, "<=");
                    token.number = tlesse;
                }

                else
                {
                    strcpy(token.sym, "<");
                    token.number = tless;
                    ungetc(ch, stdin); // retract
                }
                break;

            case '=': //state 39
                ch = getchar();

                if (ch == '=')
                {
                    strcpy(token.sym, "==");
                    token.number = tequal;
                }

                else
                {
                    strcpy(token.sym, "=");
                    token.number = tassign;
                    ungetc(ch, stdin); //retract
                }
                break;

            case '>': //state 42
                ch = getchar();
                if (ch == '=')
                {
                    strcpy(token.sym, ">=");
                    token.number = tgreate;
                }

                else
                {
                    strcpy(token.sym, ">");
                    token.number = tgreat;
                    ungetc(ch, stdin); //retract
                }
                break;

            case '|': //state45
                ch = getchar();
                if (ch == '|')
                {
                    strcpy(token.sym, "||");
                    token.number = tor;
                }

                else
                {
                    lexicalError(3);
                    ungetc(ch, stdin); //retract
                }

                break;

            case '(':
                token.number = tlparen;
                strcpy(token.sym, "(");
                break;

            case ')':
                token.number = trparen;
                strcpy(token.sym, ")");
                break;

            case ',':
                token.number = tcomma;
                strcpy(token.sym, ",");
                break;

            case ';':
                token.number = tsemicolon;
                strcpy(token.sym, ";");
                break;

            case '[':
                token.number = tlbracket;
                strcpy(token.sym, "[");
                break;

            case ']':
                token.number = trbracket;
                strcpy(token.sym, "]");
                break;

            case '{':
                token.number = tlbrace;
                strcpy(token.sym, "{");
                break;

            case '}':
                token.number = trbrace;
                strcpy(token.sym, "}");
                break;

            case EOF:
                token.number = teof;
                strcpy(token.sym, "!=");
                break;

            default:
            {
                printf("Current character : %c", ch);
                lexicalError(4);
                break;
            }
            } // switch end
    } while (token.number == tnull);
    return token;
}; //end of scanner

int main()
{
    printf("Input String : \n");
    while (1)
    {
        token = scanner();
        if (token.number == 5)
            printf("Number : %d\n", token.value.num);
        else if (token.number == 4)
            printf("Variable : %s\n", token.value.id);
        else if (token.number < 30 || token.number == 37 || token.number == 39)
            printf("Symbol : %s\n", token.sym);
        else if ((token.number >= 30 && token.number <= 36) || token.number == 38)
            printf("Keyword : %s\n", token.value.id);
        else
        {
        }
    }
    return 0;
}
