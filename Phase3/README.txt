NIKOLAOS-AXXILEAS STEIAKAKIS 5013
NIKOLAOS FERGADAKIS 5027
KONSTANTINOS GKOGKOS 4861


Apla patate make kai sthn synexeia to ektelesimo onomazetai scanner <file input>/input
Epishs ta apotelesmata ths prohgoumenhs fashs apothikeuontai sto arxeio output_parser.txt

Meriki Apotimhsh:

H diadikasia ths merikhs apotimhshs ksekinaei apo thn dhmiougria ton expression.
To challenging kommati pou antimetopisame htan protaireothta ton prakseon sta and,or,not 
Opote to lysame me thn xrhsh stoivas ,ekmetaleuntas ton kanona term apo thn grammatikh tou expression os ekshs:

1) Me to pou anagnoristei kapoio term ,dhmiourgoume ena invalid quad,(me op=-1) kai sthn synexeia to prosthetoume ston quad pinaka.
2) otan erthei h seira tou pio pano kanona tou expr,eite einai :
    expr relop expr
    expr arithop expr
    Tote sthn periptosh arithop apla kanoume lookUp me thn synarthsh  ,check_expr_quad, kai an vrethei to invalid quad ginetai valid me to neo op=arithop
    Allios an einai relop expr tote ,pali den dhmiourgeitai ekeinh thn stigmh quad,alla se ola ta pio pano epxr reqognized grammar rules me thn shanrthsh:create_relop_quad
    H opoia exei os ekshs:
        Dhmiourgia : if_eq quad
                   : Jump quad
        Sthn synexeia me thn check_expr_quad vrisko thn sosth thesh pou prepei na exoun ayat ta dyo quads kai ginetai h epanatopothetisi tous sto sosto meros
        Aksizei na shmiothei pos ginontai tautoxrona alles allages opos:
        To nextquad xorizetai pia se dyo kathgories:
        NextQuad()->Ypodhlwnei thn sosth thesh tou quad sto array of quads
        ActualNextQuad()->Ypodhlwnei thn pragmatikh thesh sto pinaka quads efoson yparxoun mesa kai ta invalid quads
    Opote otan anaferomaste sto backpatching Ean theloume label=>xrhsimopoioume to Nextquad,Allios an theloume na kanoume index to array of quads sto sosto stoixeio apla xrhsimopoio to ActualNextQuad(),
    dhladh:quads[ActualNextQuad]=NextQuad
  Auth oxi mono xrismeuei gia to backpatching alla sta while/for/if/else ,ta Na,Nb,Nc,Nd antikatoprizoun to nextquad,dhladh thn pragmatikh thesh pou thelo sto output na fanoun eno sto Nn,gia na kano index xrhsimopoioumai to ActualNextQuad 
  3)Transition to false/true lists:
     Efoson ginetai transition ton quads mesa sto array of quads gia na diathrithei h sosth seira , shmainei pos kathefora hdh yparxon true list kai false list allazoun,dhladh esto to parakato paradeigma:
     relult=a or b and c;
     ksero pos to and exei megaliterh protaireothta apo to or ,ara prota tha anagnoristei o kanonas b and c, kai meta a or expr1(b and c),alla mesa sto array of quads me thn xrhsh ton invalid quads prota yparxoyn 
     ta a b c ,opote sto or kanona molis to a anevei pano (otan ennoo anevei pano apo invalid na ginei :if_eq quad: Jump quad),to b and c den einai poia sths theshs pou htan opote tha prepei na ginei to transition ton quads mesa sto array oson
     anafora ta falselist kai truelist me thn synarthsh:"TranstionOfQuads/update_false_true_lists"
  4) efoson ola ta quads exoun dhmiourgithei sosta gia ola ta expr ,sto telos apotimeitai to teliko expression to opoio epmperiexei ola ta quads/false/true lists tou sygkekreimenou statement
  Opote opoios kanonas empriexei expr me thn xrhsh ths synarthshs:true_jump_false,dhmiourgountai ta 3 quads ,true / jump / false ,opou tha diatreksoun ta false/true lists gia na tous
  kanoun patch to label sta opoia anhkoun!
  