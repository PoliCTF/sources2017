#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int aa;
int bb;
int hs;
int ll[5000];
int zf;

typedef struct xwing_st
{
    char name[4];
    int laser_1_charge;
    int laser_2_charge;
    int laser_3_charge;
    int laser_4_charge;
    int proton_cannon_1_charge;
    int proton_cannon_2_charge;
    char pilot_name[8];
}xwing;

typedef struct z95_st
{
    char name[4];
    int blaster_1_charge;
    int blaster_2_charge;
    int ion_cannon_1_charge;
    int proton_cannon_1_charge;
    char pilot_name[8];
}z95;

typedef struct bwing_st
{
    char name[4];
    int proton_torpedo_1_charge;
    int proton_torpedo_2_charge;
    int laser_1_charge;
    int laser_2_charge;
    int proton_cannon_1_charge;
    int composite_laser_beam_charge;
    char pilot_name[8];
}bwing;

typedef struct falcon_st
{
    char name[4];
    int laser_1_charge;
    int laser_2_charge;
    int laser_3_charge;
    int laser_4_charge;
    int shields_charge;
    char pilot_name[8];
}falcon;

//////// THIS HAS TO BE BUGGY !!!!!!
void get_pilot_name(char *pilot_name,int len)
{
    printf("Please provide pilot name..\n");
    fgets(pilot_name,len,stdin);

}

void print_xwing()
{
    printf(" ____________\n|____________|-----------(--\n   ',       ',\n     ', ____  ',            ========------(- \n       ', __',  ',         .`    .`\n         ',       ',     .`    .`\n         __',_______',_.`    .`\n         | ||      --^ |_ _.]\n        \\|_ll__________|-----...,____\n       ||                 '----\"     \"\"\"\"\"-------..,.,\n       | ______________ ===========================|  \"\"-.\n       ]_| ||      '|  |    _______________________l...-\"\n         |_ll__________|.-`]\n         .`        .` ',    ',\n       .`____    .`     ',    ',\n     .`____.`  .`        ========---------(-\n   .`        .`\n .`________.`\n |___________|-------------(--\n\n");
}

void print_xwing_stats(xwing *tmp)
{
    printf("Laser 1 charge: %d  Laser 2 charge: %d\n",tmp->laser_1_charge,tmp->laser_2_charge);
    printf("Laser 3 charge: %d  Laser 4 charge: %d\n",tmp->laser_3_charge,tmp->laser_4_charge);
    printf("Proton cannon 1 charge: %d  Proton cannon 2 charge: %d\n",tmp->proton_cannon_1_charge,tmp->proton_cannon_2_charge);
    printf("Pilot: %s\n",tmp->pilot_name);
}

void print_z95()
{
    printf("        __..,,-----l\"|-.\n    __/\"__  |----\"\"  |  i--voo..,,__\n .-'=|:|/\\|-------o.,,,---. Y88888888o,,_\n_+=:_|_|__|_|   ___|__|___-|  \"\"\"\"````\"\"\"`----------.........___\n__============:' \"\" |==|__\\===========(=>=+    |           ,_, .-\"`--..._\n  ;=\"|\"|  |\"| `.____|__|__/===========(=>=+----+===-|---------<---------_=-\n | ==|:|\\/| |   | o|.-'__,-|   .'  _______|o  `----'|        __\\ __,.-'\"\n  \"`--\"\"`--\"'\"\"\"`.-+------'\" .'  _L___,,...-----------\"\"\"\"\"\"\"   \"\n                  `------\"\"\"\"\"\"\"\"\n\n");
}

void print_z95_stats(z95 *tmp)
{
    printf("Blaster 1 charge: %d  Blaster 2 charge: %d\n",tmp->blaster_1_charge,tmp->blaster_2_charge);
    printf("Ion cannon 1 charge: %d  Proton cannon 1 charge: %d\n",tmp->ion_cannon_1_charge,tmp->proton_cannon_1_charge);
    printf("Pilot: %s\n",tmp->pilot_name);
}

void print_bwing()
{
    printf("                   _____--____\n            __--^^^TT>  | |   ^^-_\n            ^^--________|_|______\\\n                    |          \\\n                  _-^           \\\n               _-^______________\\\n                 | \\          -==)\n                 | |============|\n                 |_/__________-==)\n                 ^-_           /\n                    \\          /\n                    \\         /\n                    \\         /\n                    \\        /\n                    \\        /\n                    \\       /\n                    \\       /\n                    \\      /\n                    \\      /\n                    \\     /\n                    \\     /\n                    \\     /\n                  _-^^^^-__\n            +==^^^^--____-^\n\n");
}

void print_bwing_stats(bwing *tmp)
{
    printf("Proton torped 1 charge: %d  Proton torpedo 2 charge: %d\n",tmp->proton_torpedo_1_charge,tmp->proton_torpedo_2_charge);
    printf("Laser 1 charge: %d  Laser 2 charge: %d\n",tmp->laser_1_charge,tmp->laser_2_charge);
    printf("Proton cannon 1 charge: %d  Composite laser beam charge: %d\n",tmp->proton_cannon_1_charge,tmp->composite_laser_beam_charge);
    printf("Pilot: %s\n",tmp->pilot_name);
}

void print_falcon()
{
    printf("                           ____          ____\n                          / --.|        |.-- \\\n                         /,---||        ||---.\\\n                        //    ||        ||    \\\\\n                       //     ||        ||     \\\\\n                      // __   ||        ||   __ \\\\\n                     //,'  `. ||        || ,'||`.\\\\\n                    //( \\`--|)||        ||( \'|  )\\\\\n                   //  `.-_,' ||________|| `.-_,'  \\\\\n                  //        | ||   _|   ||          \\\\\n                 //         | || | .. | ||           \\\\\n                //          | || | :: | ||            \\\\\n               /    __     888|| |  _ | ||        __    \\\n              /   ,'|_'. _ 888|| |  | | ||  _   ,'-.`.   \\\n             /   ( ,-._ | |888|| | -| | || |_| (   |==)   \\\n            /     `O__,'|_|8@ || | [| | |@ |_|  `._|,'     \\\n           /            __..|--| | )| | |--|_|__            \\           ,==.\n          /      __,--''      |  |  | |  |      ``--.__      \\         //[]\\\\\n         /    ,::::'          |-=|  | |  |::::..       `-.    \\       //||||\\\\\n        /  ,-':::::       ..::|  | /| |  |::::::::        `-.  \\     ||,'  `.||\n       / ,'    ''       ::::::|  | \\| |  |  ''::::     oo    `. \\    |--------|\n      /,'           ,'\\ :::'''|] |  | |  |   ....    o8888o    `.\\   |:::_[[[[]\n     /'           ,'   )      |> |  | | [|  ::::::     88888  ..  \\  |    |   ]\n    /           ,'     |      |] |=_| | [|  '::::'oo     8   ::::  \\-'    |---|\n   .'     o   ,'    \\_/       | :| \\| |.[|      o8888o        ::;-'      ,'   |\n  ,'     o88,'   `._/:\\|      | :|  | |: |     88888888      ,-'  \\  _,-'     |\n  |     8888 ``--'|::::|      |_:|  | |:_|       8888     ,-'   \\ ,-'   .::_,-'\n ,'    8888      ''--""      || || [| || ||            ,-'    _,-'   \\ _::'\n |                           || |_,--._| ||         ,-'  \\ ,-'     _,-'\n |                           |__(______)__|      ,-'   _,-' .::_,-'   |\n|'                  88o     =====,,--..=====  .-'   ,-' \\  _::'       `|\n|                  888888o    ,-'\\    /`-.     \\ \\-' . _,-'            |\n`---._____        888888888 ,' / [HHHH[=====-   \\ |_::'       _____.---'\n  __|_____`=======._____   /   \\ [HHHH[=====-    `'__.======='_____|__\n {I |  _______________ || /    /\\/____\\/\\:.  \\ || _______________  | I}\n [I |  :___]__[_______ ||(    /   _.._   \\:.  )|| ______]___[___:  | I]\n {I_|____________o8o___|| \\  /:  /\\  /\\   \\: / ||__________________|_I}\n    |_____,------'         \\/:'  \\ \\/ /    \\/:..   88`-------._____|\n.---'   ::::::              `.   [||||]   ,'  '::..                `---.\n|          '::..::        ,'  `-.      ,-'  `.  ':'                    |\n|.        .:::''        ,' __    ``--''    __ `.     .::.             ,|\n |        '''         ,' ,'||`.    __    ,'||`. `.    '::::.          |\n |                  ,'  (||||||) ,'||`. (||||||) 88.    '::::.        |\n `.               ,'     `.||,' (||||||) `.||,' 88888.    ':'        ,'\n  |             ,'      __       `.||,'       __  88  `.             |\n  `.          ,'      ,'||`.       __       ,'||`.      `.          ,'\n   `.       ,'       (||||||)    ,'||`.    (||||||)       `.       ,'\n     \\    ,'          `.||,'    (||||||)    `.||,'          `.    /\n      `.  |                      `.||,'                      |  ,'\n        `-|                                                  |-'\n          `.   ,'.                                    .`.   ,'\n            `-:,'o88o ,/                        \\.     `.;-'\n               `-888 //      /     ||ooo  \\ oo88 \\ __,-' \n                    `--..__ /'     ||888  `\\ oo8;--'\n                           ```-----''-----''''\n\n");
}

void print_falcon_stats(falcon *tmp)
{
    printf("Laser 1 charge: %d  Laser 2 charge: %d\n",tmp->laser_1_charge,tmp->laser_2_charge);
    printf("Laser 3 charge: %d  Laser 4 charge: %d\n",tmp->laser_3_charge,tmp->laser_4_charge);
    printf("Shields charge: %s\n",(char *)(tmp->shields_charge));
    printf("Pilot: %s\n",tmp->pilot_name);
}

void print_banner()
{
    printf("      .      .______     ______    __       __  \n   .         |   _  \\   /  __  \\  |  |     |  |      .            \n             |  |_)  | |  |  |  | |  |     |  |       .\n      .      |   ___/  |  |  |  | |  |     |  |                    . \n             |  |      |  `--'  | |  `----.|  |    .     \n_____________| _|       \\______/  |_______||__|_____________________________\n:::::::::Y8b  d88b  d8P  d88b   . 8888888b.  o88888888::::::::::::::::::::::\n\"\"\"\"\"\"\"\"\"\"88ij8888ij88' j8PY8i    888   )88  Y88<\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\n          \"8888PY8888' ,8P  Y8,   88888888'   Y8b.   .          .\n    .      Y88P  Y88P  88888888   888  Y8b_____>88b           .           \n .      .  Y8'  \"8P  d8P    Y8b  888   Y888888888P \n");
}

void print_deathstar()
{
    printf("                                    .......... ..      \n                             .....O8ZD$,+7O?$DMI ..    \n                        ....=Z,..?,,...,....O... :=M:...         \n                    .. .,....  .:....D.....:.........OI... \n                   ...O. ...   .::........=..I....I.:. .=8. ..  \n                 ..8,...     . +....N~......,:......,=.,+?~N:.. \n               ..M. ..       ...=.7::....=.+.I.?I.ZDMMMMMN88?Z+  .      \n             ..Z~ .          .......O..~~..=,..,.$MMMMMMMMMMMZN7... \n            .~Z           ..  ,:...........I... :MMMMMMMMMMMMMMM:D  \n          ..D.            . . ,,...:.,,.??.,.....MMMMMMMM?Z8:7DMM+$,. \n        . ,,                ...........~........,MNMMMMMM,ZNN~8=MMMMZ   \n       ..N?             ..........I.D,I,I..~ID$..OMDMMMMM$.7ZDDM,MMMM,... \n       .... .   .       ..I7.. .=.IMI:~..:...78.?MZMMMMMMM=Z$OMDDI7+MN. .\n       .:..,... .        +.~.  ....7M,=:,.+.$.=..:~?MMMMMMMM.~NM,ZM:MMM,. \n      .?. ... .         .Z..   ...~.:..$$,+?,.:.$8...MM:+M7NMD?==~M=DMMM..\n     .$,.?...,.         .=...  ..:Z.$Z?.+I.$D+,.:~.+8.MMM8+N.I7:DMMMMNMM.. \n   ...N...:             ,. , ...7=~778MM:=7:.:~~$:,,,.?8DMMM:ZMNMMOM8NMMM, \n   . .. ...          . ........ZINMDMMM?$NMMDM,?MM7NZZZMNMMM87MMM88MMOMMN$\n   ..~. .....    , .:$ND$DNM7MMMMMMMMMMDMMMMMMNMZMMMMMMMMMMMMMMMM8MMMMMZMM.\n   ..=. ....   . ZMMMDMNN..:.$Z~:ND....M8ZMND.MMNMMMMMM$MMMMMMMMMMMMMMNMMM.\n   .... .,NMMD, ....+...=.. .. .......,...8.MMN7MDZ.8N8MMMMD7MMNMMMMMMMMM$~\n   ...ZM.M,=  ..... .... .  ..   ..: ...~~I$MZ=8M7$M:$=NMDMMMMM8OMMMMMMMMMD.\n   :M.=,...    ..~... ..    ..    .N..:+.+:,MMDMMO:NNO+MOM$DMMMMMDI8MMMMMMM. \n . .?.. ..     . ..   .~    ..   ..M .?M=$,8MM+MMZ$+Z~~8NOIMM8NZ$MMMZMZ~MMM=\n   .... ...    . .    ..    .     .?...:?,?OMM+$M:?.,ZMMMNMDM=OONDMMIZMNMMMI\n   ...  .    .. ...   .:.   ..     N:..7...~MM.=M,~~~$7ZMDZ=.N$$.7MOMNNMMMM . \n   ...          ..      .   ..     .....=..$OZDIO7DNM:O8N$MMMMMMMMMDM:MMM8M..\n . .:..         .                  ... .  ..+OZNN.+$INMM.DN?MNMOO?NMOND+MZD.\n . .=..                     .       ... . =N8M7OMMI8+OO$~.NMIOMM7I.,OM7D88 \n   .~..            ....     ... ..   .. :~7~IDZ:NDMMMNM?NM=DN~D7?$MMMMM8M$.\n   ..:.  .. ..   .. ..:. .   ..D... . ,..IOO8M8=N7~?MM7ZDMDMD:$8D?$MMMNMM+..\n  .. M. ..  ... .        .   .D....  ........$8:$NMONIONMMMD7.M+MMN$MMMMD.\n     .N. .  .....            .,....  ..I=7..D$DM$DDON,O..D?$~MMMOMMMMMMN,.\n     .I  .   ,.     .        .,....~.,.:I.  .~..~D++N$Z$:8.MN7$NM8MMMMM?..\n    . .M.. .  :  ,.          ., ... . .:....:......7=7~=O887NNMN7MMMMMI\n      ..M ,. . ,.             ..      .=,...Z+.8MNOZO=.MO8MMMM+MMMMMMD .\n     .  .. ... ... .. ...  .. .. .7D........+NN878?M.:MMMMMM$MMMMMM MZ\n     .. . :. .  .. .. ..,,. ........  =+I:~NODZ$ZMN:.M+M8+7MMNNMMMMN~.\n        ...?     .~....~ .     ....  . :$+ON$MNM?8DNMM88~8ZMMMMMMMM .\n        .. ZM.  .. . . ..         .,:OZOZMMMZD:$MIDDM.$:DMMMMMMDM. \n            .,DD.,  ....+:....,  ,=ZMZ.??IDDZ$NNO$OMMMNDMMMMMMMMM..\n              .N,+. .~~NM$DZ7I?=M8:MMM:MZ=$MNM$OM$~M?MMMIMMMDMM~ .\n          ..   ..NMN,$D==.$MMD?7OD=:~M?7,MMMM+NMDM~DMMMMMMMMN?.  \n               .  :MMI8ZDND78DNNOMDNM8MMMMDM=:=NMMMMMMMMMMNO~..\n                   . MMMMNMMMMMMODMMINNN$MMMZMMMMZMDMMMDN~,. \n                     ..MMMMNMMMMMMMM8NNO$MM8MMMMMMMMMM.8,.\n                       . .MMMMNNMNMMMMMMMMMMMMMMMMI?D ... \n                          . ...,?8M8MMOZOM~D:? =........\n                                   ............ \n\n");
}

void print_fail()
{
    printf("                         _______    ___       __   __ \n                        |   ____|  /   \\     |  | |  | \n                        |  |__    /  ^  \\    |  | |  | \n                        |   __|  /  /_\\  \\   |  | |  |\n                        |  |    /  _____  \\  |  | |  `----.\n                        |__|   /__/     \\__\\ |__| |_______|\n\n");
}

void welcome_message()
{
    char a;
    printf("Welcome to__\n\n");
    print_banner();
    printf("\n\nGuess what.. Empire striked back, AGAIN. What could they possibly came up with this time...\n\n");
    a = getchar();
    print_deathstar();
    printf("What else could have been.. PLEASE DESTROY IT, I CANT'T STAND IT ANYMORE..\n\n");

}

int getint(){
    char input[32];
    fgets(input,32,stdin);
    // printf("Input: %s\n",input);
    return atoi(input);
}

int main_menu()
{
    int choice;
    printf("Lets do it..\n");
    printf("1. Face deathstar_\n");
    printf("2. Expand your fleet_\n");
    printf("3. Arm your fleet_\n");
    printf("4. Examine your fleet_\n");
    printf("5. Surrender_\n");
    choice = getint();
    // printf("Choice: %d\n",choice);
    return choice;
}

int expand_fleet_menu()
{
    int choice;
    printf("Expand your fleet..\n");
    printf("1. Deploy X-wing starfighter_\n");
    printf("2. Deploy z95 fighter_\n");
    printf("3. Deploy B-wing fighter_\n");
    printf("4. Request special support vehicle_\n");
    choice = getint();
    // printf("Choice: %d\n",choice);
    return choice;

}

void death_star_wins()
{
    int i = 0;
    print_deathstar();
    printf("                                   \\\\    ||    //\n");
    usleep(200000);
    printf("                                    \\\\   ||   //\n");
    usleep(200000);
    printf("                                     \\\\  ||  //\n");
    usleep(200000);
    printf("                                      \\\\ || //\n");
    usleep(200000);
    printf("                                         ||\n");
    for(i = 0;i < 30; i++)
    {
        usleep(100000);
        printf("                                         ||\n");
    }
    print_fail();
}

void face_deathstar(int *starships[],int starships_num)
{
    int choice;
    int i;
    // select how to face deathstar ( all fleet, one ship, set of ships)
    printf("Please select the starship id to attack deathstar, 0 to use all your fleet!\n");
    choice = getint();
    if(choice == 0)
    {
        for(i = 0; i < starships_num; i++)
        {
            free(starships[i]);
            starships[i] = 0;
        }
        death_star_wins();
        exit(0);
    }
    else if(choice < starships_num && choice > 0)
    {
        free(starships[choice]);
        starships[i] = 0;
        death_star_wins();
    }
    else
    {
        printf("Bad choice!\n");
    }
    // free destroyed ships
    // exit if all ships are destroyed
}

void expand_fleet(int **starship_bay)
{
    int choice;
    xwing *xw_tmp;
    z95 *z95_tmp;
    bwing *bw_tmp;
    falcon *fl_tmp;
    choice = expand_fleet_menu();
    switch(choice)
    {
        case 1:
            xw_tmp = malloc(sizeof(xwing));
            *starship_bay = (int*) xw_tmp;
            printf("Xwing deployed!!\n");
            print_xwing();
            get_pilot_name(xw_tmp->pilot_name,8);
            strcpy(xw_tmp->name,"xw\x00");
            break;
        case 2:
            z95_tmp = malloc(sizeof(z95));
            *starship_bay = (int*) z95_tmp;
            printf("Z95 deployed!!\n");
            print_z95();
            get_pilot_name(z95_tmp->pilot_name,8);
            strcpy(z95_tmp->name,"z95;");
            break;
        case 3:
            bw_tmp = malloc(sizeof(bwing));
            *starship_bay = (int*) bw_tmp;
            printf("Bwing deployed!!\n");
            print_bwing();
            get_pilot_name(bw_tmp->pilot_name,8);
            strcpy(bw_tmp->name,"bw\x00");
            break;
        case 4:
            if(hs && !zf)
            {
                fl_tmp = malloc(sizeof(falcon));
                *starship_bay = (int*) fl_tmp;
                printf("Chewie... we're home...\n");
                print_falcon();
                strcpy(fl_tmp->pilot_name,"han\x00");
                strcpy(fl_tmp->name,"fl\x00");
                zf = 1;

            }
            else if(zf)
            {
                printf("One falcon is enough, deploying two could be fun... but useless..\n");
            }
            else
            {
                printf("I'm sorry to report commander Han Solo is unreachable at the moment..\n");
            }
            break;
        default:
            printf("Bad option!\n");
            break;
    }
    fflush(stdin);

}

int modstats(int *starship)
{
    char *starship_type = (char *)starship;
    printf("Starship type: %s\n", starship_type);
    int malefic_malloc;
    int next_chunk_size_addr;
    int next_chunk_size;
    int new_next_chunk_size;
    xwing *xw_tmp;
    z95 *z95_tmp;
    bwing *bw_tmp;
    falcon *fl_tmp;
    if (strcmp(starship_type,"xw") == 0) 
    {
        // ask for xwing stats
        next_chunk_size_addr = (int) starship + (4*9);
        next_chunk_size = *(int *)next_chunk_size_addr;
        xw_tmp = (xwing *) starship;
        printf("Please input laser 1 charge:\n");
        xw_tmp->laser_1_charge = getint();
        printf("Please input laser 2 charge:\n");
        xw_tmp->laser_2_charge = getint();
        printf("Please input laser 3 charge:\n");
        xw_tmp->laser_3_charge = getint();
        printf("Please input laser 4 charge:\n");
        xw_tmp->laser_4_charge = getint();
        printf("Please input proton cannon 1 charge:\n");
        xw_tmp->proton_cannon_1_charge = getint();
        printf("Please input proton cannon 2 charge:\n");
        xw_tmp->proton_cannon_2_charge = getint();
        get_pilot_name(xw_tmp->pilot_name,12);
        new_next_chunk_size = *(int *)next_chunk_size_addr;
        if((unsigned int)new_next_chunk_size > (unsigned int)next_chunk_size)
        {
            printf("My young padawan... force is not with you this time..\n");
            *(int *)next_chunk_size_addr = next_chunk_size;
            return 1;
        }
        else return 1;
    }
    else if(strcmp(starship_type,"bw") == 0) 
    {
        // ask for bwing stats
        next_chunk_size_addr = (int) starship + (4*9);
        next_chunk_size = *(int *)next_chunk_size_addr;
        bw_tmp = (bwing *) starship;
        printf("Please input proton torpedo 1 charge:\n");
        bw_tmp->proton_torpedo_1_charge = getint();
        printf("Please input proton torpedo 2 charge:\n");
        bw_tmp->proton_torpedo_2_charge = getint();
        printf("Please input laser 1 charge:\n");
        bw_tmp->laser_1_charge = getint();
        printf("Please input laser 2 charge:\n");
        bw_tmp->laser_2_charge = getint();
        printf("Please input proton cannon 1 charge:\n");
        bw_tmp->proton_cannon_1_charge = getint();
        printf("Please input composite laser beam charge:\n");
        bw_tmp->composite_laser_beam_charge = getint();
        get_pilot_name(bw_tmp->pilot_name,12);
        new_next_chunk_size = *(int *)next_chunk_size_addr;
        if(new_next_chunk_size != next_chunk_size)
        {
            printf("My young padawan... force is not with you this time..\n");
            *(int *)next_chunk_size_addr = next_chunk_size;
            return 1;
        }
        else return 1;
    }
    else if(strcmp(starship_type,"z95;") == 0) 
    {
        // ask for z9 stats
        next_chunk_size_addr = (int) starship + (4*7);
        next_chunk_size = *(int *)next_chunk_size_addr;
        z95_tmp = (z95 *) starship;
        printf("Please input blaster 1 charge:\n");
        z95_tmp->blaster_1_charge = getint();
        printf("Please input blaster 2 charge:\n");
        z95_tmp->blaster_2_charge = getint();
        printf("Please input ion cannon 1 charge:\n");
        z95_tmp->ion_cannon_1_charge = getint();
        printf("Please input proton cannon 1 charge:\n");
        z95_tmp->proton_cannon_1_charge = getint();
        get_pilot_name(z95_tmp->pilot_name,12);
        new_next_chunk_size = *(int *)next_chunk_size_addr;
        if(new_next_chunk_size != next_chunk_size)
        {
            printf("My young padawan... force is not with you this time..\n");
            *(int *)next_chunk_size_addr = next_chunk_size;
            return 1;
        }
        else return 1;
    }
    else if(strcmp(starship_type,"fl") == 0) 
    {
        // ask for falcon stats
        next_chunk_size_addr = (int) starship + (4*8);
        next_chunk_size = *(int *)next_chunk_size_addr;
        fl_tmp = (falcon *) starship;
        printf("Please input laser 1 charge:\n");
        fl_tmp->laser_1_charge = getint();
        printf("Please input laser 2 charge:\n");
        fl_tmp->laser_2_charge = getint();
        printf("Please input laser 3 charge:\n");
        fl_tmp->laser_3_charge = getint();
        printf("Please input laser 4 charge:\n");
        fl_tmp->laser_4_charge = getint();
        printf("Please input shields charge:\n");
        fl_tmp->shields_charge = getint();
        get_pilot_name(fl_tmp->pilot_name,32);
        printf("Force is strong in this one.. please tell me, how much?\n");
        malefic_malloc = getint();
        if(malefic_malloc != 0)
        {
            malloc(malefic_malloc);
        }
        new_next_chunk_size = *(int *)next_chunk_size_addr;
        return 1;
    }
    else return 0;
}

void arm_fleet(int *starships[],int starships_num)
{
    int selection = 0;
    printf("Please select starship index:\n");
    selection = getint();
    if(selection >= 0 && selection < starships_num)
    {
        modstats(starships[selection]);
    }
    else
    {
        printf("Bad index!\n");
    }
}

void examine_fleet(int *starships[50000])
{
    int i = 0;
    char *name;
    for(i = 0 ; i < 50000 ; i ++)
    {
        // printf("Ciclo: %d --> %p\n",i,starships[i]);
        if(starships[i])
            name = (char *) starships[i];
        else continue;
        if(!strcmp(name,"xw"))
        {
            printf("Ship [%d]: Xwing\n",i);
            print_xwing();
            print_xwing_stats((xwing *)name);
        }
        else if(!strcmp(name,"z95;"))
        {
            printf("Ship [%d]: Z95\n",i);
            print_z95();
            print_z95_stats((z95 *)name);
        }
        else if(!strcmp(name,"bw"))
        {
            printf("Ship [%d]:  Bwing\n",i);
            print_bwing();
            print_bwing_stats((bwing *)name);
        }
        else if(!strcmp(name,"fl"))
        {
            printf("Ship [%d]: Falcon\n",i);
            print_falcon();
            print_falcon_stats((falcon *)name);
        }
        else continue;
    }
}

int main(int argc, char* argv[]) 
{   
    int i;
    aa = 0x00000000;
    bb = 0xffffffff;
    hs = 0;
    for (i = 0 ; i <5000 ; i++)
    {
        ll[i] = 0xffffffff;
    }
    zf = 0;
    // print_xwing();
    // print_z95();
    // print_bwing();
    // print_falcon();
    setvbuf(stdin,0,2,0);
    setvbuf(stdout,0,2,0);
    int starships_num = 0;
    int *starships[50000];
    int *heapset;
    int heapcalc;
    int *bothan;
    int choice;
    memset(starships,0,50000*4);
    
    heapset = malloc(1);
    heapcalc = (int)heapset & 0xFFF00000;
    heapcalc = heapcalc + 0x100000;
    printf("Initial heap: %p, malloc of %d\n",heapset,heapcalc - (int)heapset);
    while(heapcalc - (int)heapset > 0x500)
    {
        heapset = malloc(128);
    }
    bothan = malloc(1);


    welcome_message();
    while(1)
    {   
        choice = main_menu();
        switch(choice)
        {
            case 1:
                face_deathstar(starships,starships_num);
                break;
            case 2:
                expand_fleet(&starships[starships_num]);
                starships_num ++;
                break;
            case 3:
                arm_fleet(starships,starships_num);
                break;
            case 4:
                examine_fleet(starships);
                break;
            case 5:
                exit(0);
                break;
            case 6:
                printf("Many Bothan hackers died to bring us this information. %p  -  %x\n",bothan,*(bothan + 3));
                break;
            default:
                printf("Bad option!\n");
                break;
        }
    }

    return 0;
}