# Entrée neutre — revue indépendante nativeRead/contrôles

2026-10-26. Revue source seule, aucune édition/shell/compilation/test/délégation. Vérifier profil correctness-reviewer natif et contexte **fresh sans histoire auteur** ; lecture et restitution puis attendre contrôlelead avant analyse. Canonique ARTIST explicitementautorisé, aucun accès externe nécessaire. Une erreur/refus = restitution précise, pasbootstrap/grant/retry.

## Contrat/candidat

ProgrammeWin32autonome, pas moteur/Reader/GL. Candidat helper intégré961d92ed06904e6d97b0ac5969aba6f8d205e21c ; HEADdocpeutensuiteavancer sanssourcechanged. Lire seulement `scripts/probes/qa-f8-stdio/native_read.hpp` (63lignes) et `controls.cpp` (111lignes) ; refs/greep sousarbreciblé siutile, aucunebase decompilationduprobe promise. Ne pas lire RED-PROTOCOL.md, rapports/justifications auteurs, boardimplementation ni remisesd'autresreviewers. Contextecontrat/proofci-dessous suffit, pasdeQMD/ContextModeCLI/lectureexterne requise.

SHAhelperc75d648542b27f2511417b27478ad7bb438b85d66469edafbdd7a6609b91a026 ; contrôlesed9e0e865d2f48c67f9294a7a05bce52bfeadc5c60545bc36f82f3805f8517f8. Baseline sourcehelperc77fb5a disponible parhistoireGit mais aucungitcommandereviewer demandé ; corpscourant suffit aucontrat (ancienhéritageunelecture8192neconvenaitpasaucontratnouveaude12325).

Invariants : lecturecomplète jusqu'à ReadFileréussi/n0, lecturecourt≠EOF ; n=taillebytes (pascompteurDWORDtotal tronqué), donnéesdesreadséchoués nonpubliées, prefixesuivéok=false/erreurcapturée. HANDLElocalpossédéunique, closeunefoistouscheminsC++, erreurprimairepréservée si cleanupéchoue, closeéchouéaprèslecturecorrecteinvalidecontrôle ; aucunborrowasync/concurrence. Contrôlescomparentlongueur ET stringcomplète pour0/4096/8192/12325, nullepartstrlen/statdéfinissantattendu. RootabsolueNEUVE sousparentfiablepossédé/sansmodificateurconcurrent ; nomsCREATE_NEWuniques ; cleanupseulsnomsacquis/rootacquise, pasrécursion/voisins. CasCLInonreconnu/refusrootpréexistante avantacquisition, aucunedestructionempruntée.

## Preuves runner attribuées lead, pas exécutées par reviewer

BuildMSVC14.44Debug0, puis mêmescontrôlesimmuables0/4096/8192/12325 tousoctetsexactsReadBOOL1/error0/exit0 ; dernierfichier8192+4133+0. DeuxprocessusdistinctsCLI : rootpréexistante+sentinellepréservéesetcaseinconnu/rootjamaiscréée, exit20attenduschacun. Source/exeavantaprès/sentinelles/cleanupvérifiés.4assertionshelper+2contratsCLI, pas6CTest/GTest ou preuveGPU.

GenuineReadFileerror,closeerror/allocationnonexécutés, DWORDOSinternemapping/CRTstdio/Reader/catchnonprouvés. Archiveslead : out/qa-f8/stdio-green-20261026-01/{candidate-before-run.json,verification-summary.json,whole-read.log,existing-root.log,invalid-case.log}. DocumentsMeta JSON seuls autorisés pourcorroboration siutile : docs/_meta/session-2026-10-26-stdio-control-{red,green}.json. Aucun besoin des rapportsauteurs ou rechercheshistoriquesB1/C2.

## Livrable

Findings prouvés UB/ownership/lifetime/exception/conversions/contrôles avecfichier:ligne,symbole,scénariodiscriminant etimpact ; passtyleoufaussegarantie exhaustive. Distinguerbugactuel danscecontrat, suggestionfuturehorsscope etbranchepreuveabsente. Hypothèsehaute exigeraiscénarioréalistecompatibleowner/périmètre avantfinding, paspipelineGPU ouperfdemandés. Retourtextefragments<3500octetsUTF8/FIN_REVIEW_NATIVE, leadpersiste ; aucun fichiers/tâchescomplétésparreviewer. Verdict borné, pasautotest ni indépendance depuisseulnomdeprofil.
