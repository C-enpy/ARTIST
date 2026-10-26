# 72 — Proposition stdio complète, avant toute nouvelle source

2026-10-26. Cinq fragments S_PROBE_RO fond et FIN reçus de pool8 ; lectures44/46/48/QA17 relatives,69digestionlead. Même auteurhelper/contexteconservé nonindépendant, admission70 levée. Aucune édition/commande/build/test/API/processus/flag/handle. Propositions ci-dessous non encore autorisation source, task en cours.

## Interface, issues et périmètre

Conforme48 : probe.exe --case m0|m1-text|m1-binary --root ABS_NOUVEAU, ordre strict ; troisprocessus/racines distincts, M2 phase recovery des deuxm1, pas mode supplémentaire. HeaderhelpervalidéSHA c75d648542b27f2511417b27478ad7bb438b85d66469edafbdd7a6609b91a026 etcontrolsSHAed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8 immuables.

0=OBSERVATION_VALID_FAVORABLE,10=VALID_UNFAVORABLE,20=INCONCLUSIVE_PRECONDITION,30=INVALID_INSTRUMENTATION_OR_CLEANUP ; chaque ligne conserve issue/motif, agrégation30>20>10>0 sans effacer les lignes défavorables. CLIinconnue/rootexistante refusées20avantacquisition. Aucun0global si ligne nonconforme/inconclusive, pasrésultatCRT favorable forcé. M3/closeprotection/catch/Reader/publication horsscope, modesReader nonadoptés.

## M0 : matrice proposée déterministe

10vecteurs×3classeschemin=**30lignes**, pas sweep. BaseLF : vide0,courtASCII«void main() {}»+LF,4096/8192/12325/4608 répétant«0123456789abcdef»+LF puis cinqderniersoctets remplacés par LF END LF. Quatrevariants indépendants4608 : CRLF auxoffsets4095/4096,CtrlZ0x1Aà17,BOMUTF8aux0..2sanschangertaille,NULà17 ; suffixe préservé. Aucune combinaison cachant unmotif.

NomsASCII/espace/nonASCIIreprésentablesdansACPcourant sanssubstitution/bestfit, conversionallerretourvérifiée ; ACP/localeCRT/nomUnicode/octetspath consignés avantlecture, passetlocale/_setmode/ccs/changementcodepage. Nomnonreprésentable/ouvertureimpossible=ligne20,pasPASSskip ni rechercheillimitée.

CREATE_NEW/WriteFileexact/Flush/Close avantlectures ; vecteursattendus indépendants. Quatrelecteurs frais : nativeReadcomplet, extracteurproductionifstreamtexte (identitéàfixer73), fopenr,fopenrb. Collectesblocs4096/size1/concatparlongueur,NULnontronqué,retours/états/close visibles. BRUTnative/rb exacts ; BaseLFtexte/r/rb exacts ; spéciauxrcomparé au baselineproductiontexte **observé**, CRLF/CtrlZ nonprésumés. Divergence=10,pastoléranceélargie ; identité/collecteinvalide n'est pascompatibilitéfavorable. Logs : tailles/premieroffsetdifférence/hexattendureçu/dumpscompletsutiles, hashcomplémentseulement, pasNULambigu.

## M1 : ordre de l'expérience discriminante

Deuxmodesr/rb,sousracinesnouvellesBaseLF4608. Fopenneufréussi,pos0,feof/ferror0,sanslecturepréalable/clearerr/fseekréparateur,errnoinitialdescriptif. Handlelockowner séparé,shareREAD|WRITE|DELETE,LockFileExEXCLUSIVE|FAIL_IMMEDIATELYplage[0,4608),OVERLAPPEDvivantjusqu'unlock.

Handletemoinneufpos0 : uniqueReadFile4096recouvrant,snapshotBOOL/GLE/nimmédiatavantclose/log. ExigerBOOL0/error33/n0 ; sinon20non-discriminant,sansverdictstdiofavorable. Erreurclosetémoin=30secondairesansmasquerprimaire. Verrouactif : premièreunique fread(buffer,1,4096), sauver n/errno puisferror/feofavantcleanup/log. Donnéesreçuesmêmeinattenduesgardéesparlongueur ; pasbouclepourchercherautreerreur ni flag/throw/handleinvalide injectés.

Témoin33+n0/ferror!=0→0distinctionerreur,mêmefeofnonzero ; témoin33+n0/ferror0/feof1→10optiondéfavorable ; n0sansindicateur ou n>0inattendu→20anomalieàinstruire,pasM3autorisé ; snapshotincomplet/aprèscleanup→30. Errno seul n'est pasoracle. Primaire visible même si recovery/cleanupéchouent. Aucunepreuvecloseéchoué/catchReader.

## M2 et ownership

AprèssnapshotM1 : uniquefclosefluxM1,retour/errnoimmédiats,aucunretry/réutilisationFILE*/clearerr ; pasextractionhandleCRT. Unlockexplicitemêmeplage/OVERLAPPED,diagnosticBOOL/GLE, puis **nouveaufopenmêmemode** lecturejusqu'EOF exact4608 etcomparaisonM0 ; nativeReadexact aprèsunlock. PasréparationdufluxM1fermé revendiquée. Recoveryfaux/étatimpossible=30,primaireM1conservé.

Acquisitionrootexclusiveparentfiable,registreseulsnoms/handles/FILE/lockréellementacquis,enregistrement sansallocationperdantowner,destructioninverse ; unlockavantcloselockowner,OVERLAPPEDvivant. Cleanupnomsacquis+RemoveDirectorynonrécursif,pasroottiers ; closeunefois,erreurcleanupsecondaire30sansmasquerprimaire,unwindallocationpréservé,pastroncature. Runnerowns sentinelleextérieureprépost/SHA/logshorsfixtures ; sousracineabsenteaprèsprocessus ou inventairerésiduel,paspurgeparent.

Pré-run : vraisSHAcandidat/helper/controls/exe/outils/env/entréeCL/vecteurs/nomsACP/locale,sélection3cas,argvexactsdérivésaprèsintégrationetapprouvés ; unbuildsortieneuve, pasCTest/ASan/GL/install/infrastructure. AnciensCLIguards seulementàleurcandidat, pas transfertautomatiqueauprobe.

## TDD et gatebaseline explicites

HelperRED/GREEN58/62neprouvepasTDDfuturprobe. RésultatsCRT/verrou/recovery inconnus=caractérisations46, pas obligationrougir/réparer. Nouveauxcontrôlesdéterministes proposés **avant leur implémentation** : tailles/offsets/suffixegénérateurs,comparevide/NUL/longueurs/diffaprès4096/8192,agrégation/lines,CLIrootrefus. Attenduslittéraux/positionsexplicites etoctetsfixture,pas générateurappelédeuxfois/mockOS. Runnerlead après intégration ; aucun nouveauREDobservé, assertiondéjàverte sansrougecausal reste GAP àporter,pasdéfautvolontaireinventé.

Gate signalé : baselineexige extractShaderSource inchangé, pasloopifstreaméquivalentprésumé. Corps/accessibilité manquants au brief : oracletexteINCONCLUSIVE avantdigestion73, aucune sourceautoriséepourcettepartie. Lesrecettes30lignes/classification et nouveauxcontrôlesdemandent adoptionavantcode ; pasnouvellecampagneimplicite.

Remise complète textuelle seulement, sourcefuture sousstdioexclusif, aucunmoteur/Reader/messages/catch modifié. Tasknonclôturée ; prochainedécision aprèsvérificationbaseline73 et regroupementaveclimiteTDDparent71.
