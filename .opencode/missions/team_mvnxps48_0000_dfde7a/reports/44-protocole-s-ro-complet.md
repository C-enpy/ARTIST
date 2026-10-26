# 44 — S_RO : protocole complet proposé, sans source ni essai

2026-10-26. Auteur pool-5/cpp-engineer, sept fragments S_RO et FIN_S_RO reçus. Persistance lead ; contexte E1 conservé/non indépendant conformément41. Aucune édition/compilation/exécution/build/handoff/claim/délégation/installation. Même tâche S encore ouverte pour sonde future ; remise RO seulement, pas source produite.

## Entrées et identité

37/38/34/39/40/41, contratB3, QA17amendé, ancien25 et sourcearchive lus. Sourcearchive présente au canonique. Ref main lu46ce6d7590a607ce39ae82cf7228fea756b2f5b9, pas worktree isolé4b0626e prétendu. Source82bf52d/delta pertinent doivent être recoupés par lead avant auteur/runner. Isolation auteur toujours bloquante. Aucun fichier prototype/contrôle créé.

## TDD étroit et caractérisation distincte

L'erreur fread sous verrou et la compatibilité texte sont inconnues à **caractériser**, pas un bug de sonde à fabriquer. Ancien25 est négatif causal Reader, pas RED du probe stdio. Pas de comparateur volontairement faux/throw/flags/handle invalidé.

Capacité manquante concrète proposée : nativeRead de l'ancienne source71–81 fait un seul ReadFile dans8192octets. Pour nouveau contrat M0 contenu exhaustif12325, il retourne8192. Ce n'est pas un défaut de l'ancien contrat4608 ; adoption du nouveau contrat helper nécessaire. Test exhaustif0/4096/8192/12325 d'abord sur helper hérité inchangé ; RED attendu12325→8192 alors que ReadFile a réussi, témoins0/4096/8192 verts. Puis même auteur adapte boucle lecture complète jusqu'EOF authentique/erreur ; GREEN mêmes assertions exactes et mêmes octets. Pas fstat ni sizecheck de correction Reader, ancien fichier archivé intact.

Cette tranche ne prouve pas TDD de toute la nouvelle sonde : parties purement exploratoires M0traduction/M1/M2 requièrent traitement explicite avant code, jamais une exemption implicite ou un faux RED moteur. Si tranche helper rejetée, pas nécessité de l'ajouter pour contourner TDD ; maintenir gap et décider caractérisation. Auteur isolé et runner distinct avant toute source/exécution.

## M0 — fixtures et oracles

Racine nouvelle possédée, vecteurs/octets déterministes figés dans manifeste pré-run, écriture Win32 sans traduction, WriteFile exact et FlushFileBuffers. BaseLF :0, court «void main() {}»+LF,4096,8192,12325,4608 ; grands motifs ASCII cycliques/LF fixés. Variants4608 **séparés** avec suffixe reconnaissable : CRLF traversant4095/4096, Ctrl-Z0x1A à17, BOM UTF8 initial, NULà17. Ne pas combiner Ctrl-Z avec autres motifs qu'il cacherait ; aucun shader compilé.

Chemins ASCII, espaces, nonASCII représentable sans substitution dans encodage étroit courant ; figer octets nom/ACP/localeCRT/Unicode observé et hash avant run. Aucun setlocale global/_setmode/ccs= ni promesse UTF8. Non-représentabilité/refus d'ouverture = INCONCLUSIVE de cette ligne, pas skip/PASS ou changement de page de code.

Trois flux frais indépendants par cas : ifstream texte via extractShaderSource inchangé, fopen r, fopen rb ; lecture blocs4096/size1 ; octets/longueur complets, états finaux, appels/retours et close observés. Pas appel Reader/Loader. OracleBRUT : nativeRead complet et fread rb exacts au vecteur initial, NUL compris, jamais strlen. LF : trois voies exactes. Spéciaux : fread r comparé au ifstream texte caractérisé ; observation brute distincte, CRLF→LF/Ctrl-Z→fin restent hypothèses et non prétexte à comparer brut et texte identiquement. BOM/NUL non masqués par logs. Divergence texte = compatibilité défavorable portée à D, pas tolérance élargie. Taille/premier octet divergent/dumphex exact, hash seul insuffisant.

## M1 — erreur native authentique

Fixture LF4608 stable, handle propriétaire avec FILE_SHARE_READ|WRITE|DELETE. Pour r puis rb seulement, sous-cas distincts sans sweep :
1. fopen neuf réussi, aucune lecture préalable, position0/feof=ferror=0 observés sans clearerr/fseek réparateur ; ordre enregistré.
2. Handle séparé : LockFileEx EXCLUSIVE|FAIL_IMMEDIATELY sur[0,4608), OVERLAPPED vivant jusqu'unlock, acquisition confirmée.
3. Nouveau handle témoin, position0, ReadFile4096 recouvrant ; BOOL/GetLastError/n immédiatement avant close témoin. Exiger erreur33/n0. Échec open/verrou ou absence33 = non discriminant/INCONCLUSIVE, jamais résultat stdio favorable.
4. Première et unique fread(buffer,1,4096,flux), sauver immédiatement n/errno puis ferror/feof avant cleanup/log. errno avant opération également enregistré, descriptif seulement. Aucun flag/EOF/throw forcé ni boucle pour chercher une erreur tardive.
5. Témoin33 + ferror!=0 : distinction erreur (prioritaire même avec feof). Témoin33 + n0/feof1/ferror0 : option stdio **défavorable**, expérience valide, pas code à réparer pour vert. Zéro sans EOF/erreur ou octets inattendus = anomalie à instruire ; n>0 inattendu conservé sans prétendre M3. Snapshot post-cleanup/log incomplet invalide preuve.

Pas de publication Reader, de preuve catch ou de close échoué. Les modes r/rb sont deux hypothèses, pas répétitions aléatoires.

## M2 — recovery, lifetime et cleanup

Après snapshotM1 fermer FILE* une fois, sauver retour/errno distincts ; fclose consomme flux même échoué, aucun retry/usage ultérieur/clearerr pour modifier M1. Owner unique local ; préserver primaire et secondaires. Unlock explicite puis **nouvel open** recovery, lecture jusqu'EOF et comparaison au M0même mode ; LF attendu4608. ReadFile complet exact également. Pas remise en état du FILE*M1 fermé revendiquée.

CREATE_NEW/CreateDirectory refusant l'existant, registre local ressources acquises, destruction inverse, handles et OVERLAPPED vivants ; pas remove_all(parent)/racinepréexistante. Erreur allocation/cleanup observée sans masquer primaire ; pas succès silencieux. Runner garde sentinelle hors sous-racine avec octets/hash avant/après ; logs hors fixtures. Contrôle après processus : sous-racine absente, voisin et sortie intacts.

Contrôles légitimes proposés séparément : root déjà existante possédée par runner/témoin intact → refus sans suppression ; argument/mode inconnu → refus avant acquisition. Ce ne sont pas erreurs OS de lecture/close et ne valent RED que si défaut réel constaté. Adoption préalable, pas multiplication implicite essais.

## Dérivation compilation réellement lue, non exécutée

Entrée unique TraceableExceptionTests.cpp dans out/build/windows-msvc-debug/compile_commands.json, directory=out/build/windows-msvc-debug. cl observé sous VS18 Community/MSVC14.44.352 ; /TP,-DGLEW_STATIC,-D__mock_gl__,12includes,-external:W0,/DWIN32,/D_WINDOWS,/EHsc,/Ob0,/Od,/RTC1,-std:c++latest,-MDd,-Zi,/FS. Aucun retrait de protectionDebug.

run-build.py charge msvc-env.cmd actuel : VS18Community x64 -vcvars_ver=14.44, pas ancien VS2022BuildTools à recopier. Wrapper cwd racine source, argv[2:] exécuté, affiche COMMAND/EXIT, stdout/stderr **fusionnés** dans out/logs/tag.log. L'ancien build.log25 ne prouve pas sa commande intégrale.

Noms proposés non existants : scripts/probes/qa-f8-stdio/probe.cpp,controls.cpp/helper local. Nouvelle sortie O=out/qa-f8/stdio-ID absente initialement ; P=out/tooling/build/Scripts/python.exe,W=out/tooling/run-build.py. Découper commandeCL par CommandLineToArgvW comme25 ; conserver tokens sauf -c,/Fo,/Fd et source originale ; source intégrée absolue,/FoO/nom.obj,/FdO/nom-compile.pdb,/FeO/nom.exe,/link /DEBUG /PDB:O/nom.pdb. Option inattendue = arrêt pour recoupement, pas ligne devinée ni nouvelle lib. Un seul build par sortie ; auteur ne compile jamais.

Commandes futures proposées, **pas lignes exécutables validées** : P W tag-build-nom CLexact argv-dérivé-exact ; P W tag-controls O/controls.exe --case native-read-whole --root O/owned-controls ; P W tag-m0 O/probe.exe --case m0 --root O/owned-m0 ; sous-cas m1-text et m1-binary avec racines propres. CLI/codes d'issue favorables/défavorables/non-discriminants restent à adopter avant code ; ne pas réutiliser exit10historique ambigu. TagsRED/GREEN distincts, tests identiques, archives25 intactes.

Pré-run : HEAD/delta possédé,SHA sources+headers dont Reader,compile_commands/entrée,wrapper/env,binaire,fixtures/vecteurs attendus,ACP/locale,env/sélection exacte. Post-run : mêmes identités,codes,logscomplets/diagnostics finaux. Un run reste une preuve malgré plusieurs artefacts. Pas CTest/ASan/GPU pour cette sonde autonome ; deux essais sans info imposent hypothèse nouvelle/INCONCLUSIVE. M3exclu,catch/close ouverts,ADR inchangés.

## Décisions restantes

Isolation native auteur ; adoption contrat whole-native-read/TDD étroit ; traitement explicite caractérisations sans RED fictif ; CLI/codes et argv exacts avant runner. Aucune de ces décisions ni implémentation n'est présumée par la remise. Source/test futur ne reprend pas automatiquement les verdicts ASan555. S reste in_progress tant que son livrable source prévu n'est pas intégré ; le protocole RO seul peut être acté sans falsifier le board.
