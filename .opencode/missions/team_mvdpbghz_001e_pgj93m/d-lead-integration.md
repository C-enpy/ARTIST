# D — intégration et recoupement du lead, avant G/R

## Identité et barrière d'intégration

Producteur : pool-6 / gpu-engineer, tâche task_mvdy0o26_000s_jn4jct.
Commit auteur :29ee2f368a5e63771368bcfa18de6b31de599d68,
base :1f1ad63b3a4d9d5819845eeb135275beaee4234c.
Worker arrêté proprement, commit conservé par Ensemble. Intégration uniquement
par team_merge, pas de git merge manuel. Les17 fichiers intégrés ont été comparés
au manifeste auteur : SHA256 bruts ET blobs Git identiques,0divergence.
Diff --check moteur/tests : succès. Quatre headers de production,tests/mock,
sept shaders QA ; pas Binder/Unbinder/Loader/Pass public modifiés.
Relecture lead des quatre changements de production effectuée, sans prétendre
constituer une revue indépendante. Le gate générique est limité au contrat
HasBorrowedOnMethod ; les doubles de réflexion sont locaux à l'invocation.

Serena initialisé sur la racine ARTIST correcte, mais aperçu Setter trop
superficiel et recherche du symbole on vide sur ces templates. Lecture ciblée
du diff/fichiers complétée ; aucune garantie d'analyse C++ complète déduite du LSP.

## Incident d'index externe traité avec autorisation explicite

team_merge a d'abord refusé une note locale de preuves, committée séparément
9a1462d ; puis son autostash a échoué sur deux entrées intention d'ajout externes.
Pas de conflit de code moteur et aucun MERGE_HEAD. L'utilisateur a autorisé le
retrait temporaire et le rétablissement des seuls marqueurs ITA de :
.opencode/evidence/qa-tools-policy-2026-10-18/validation.md et verify.mjs.
État/index sauvegardés sous out/attribute-gl/git-integration/.
Retrait via git reset -- sur ces deux chemins seulement,team_merge réussi,
restauration git add -f -N sur ces seuls fichiers (ils sont ignorés dans ce repo).
Empreintes de contenu avant/après identiques,statuts et flags20004000 rétablis,
aucune donnée externe committée avec le moteur. Pas de reset destructif/stash
manuel/édition globale ni contournement d'un refus d'accès.

Empreintes externes préservées :
- validation.md :97da769d2a08509b01d71e7ed34b813e7378cae96238e9887b403e22749da7f0 ;
- verify.mjs :083958de4ac23d30f17c98b64e3e6d8132743cbc8eaa391bcad9203aee1cd82c.
Autres changements externes harness/profils/AGENTS/docs/.grepai laissés hors lot.

## Artefacts intégrés dans le périmètre de lecture des consommateurs

Snapshot :out/attribute-gl/author-d-29ee2f3/author-evidence.json,
SHA256 vérifié e70a1f964f823471a336782b485d9917a8c8fee0beb25047bdc57f6b921e8396.
87 artefacts sélectionnés (logs/JUnit/commandes/résultats/captures/patch) copiés
byte à byte depuis le Temp approuvé vers le sous-dossier logs ; chaque SHA256
source et copie recoupé avec le manifeste. Aucun helper d'environnement ou
fichier secret importé. Pas d'accès au log long de worktree précédemment refusé.
Les chemins absolus du manifeste restent ceux du producteur ; correspondance
locale :tout suffixe après /logs/ se trouve sous le snapshot logs/.

Binaires originaux vérifiés par lead :
- unit8054784octets,SHA2560455bbfb3bf39246c76bc96316992bca7f253379aa55359ed4a3387666d26fd6 ;
- GL4680704octets,SHA256f385df592edf02c916ce4c2414c72c5dc44f25180a872ce37c26346149e14c34.
Les exécutions ci-dessous sont celles de l'auteur ; le lead a recoupé leurs
artefacts, pas encore relancé le candidat depuis la racine intégrée.

## Résultats recoupés, pas de PASS global

JUnit parsés :266unit/266cas/0échec,60GLfonctionnels/60cas/0échec,
conformitéstruct1cas/1échec ;0skip/disabled partout. Union exacte327noms uniques.
326PASS fonctionnels +1FAIL de conformité NVIDIA617.42 conservé, selon arbitrage
humain d854816 ; jamais résumé en327/327PASS.
Le seul nom non sélectionné du run fonctionnel GL est celui de conformité,
exécuté séparément sur candidat final. Ancien JUnit initialFAIL conservé.

Lead a reconstruit indépendamment les120octets attendus depuis les12payloads du
contrat (écritures float32/int32/uint32 little-endian Windowsx64), sans utiliser
les fichiers expected de l'auteur comme oracle. Lecture readback136octets :
payload120 strictement identique et16sentinelles0xA5 intactes. Separate :
payloads4/4/4/16 aux offsets source0/40/80/24 identiques au contrat,
readbacks20/20/20/32 avec16sentinelles intactes chacun.
Cette analyse vérifie les captures conservées ; contexte/commandes/query et
interface qui les ont produites restent aussi à contrôler dans les revues.

RED/GREEN :JUnit de Reader6/9CPU et2/2natifs échoués,puis12/12GREEN ;
Setter4/4CPU et3/3natifs échoués (sans draw/TF sur interface invalide),puis19/19GREEN.
Log de compilation :trois C2607 aux assertions de fermeture de famille,
pas un problème d'outillage. Non-régression reload1/60GL échoué,régression CPU
dédiée1/1RED puis11/11GREEN après jeu de noms local Reader. Les diagnostics
et les symboles en échec ont été relevés par lead depuis les artefacts.

Limite d'archivage :les anciens binaires RED/baseline ont été écrasés par les
builds incrémentaux. Logs/JUnit conservés,mais pas d'empreinte binaire historique
pour chaque transition. Ne pas présenter les preuves comme une chaîne complète
de binaires archivés ou une attestation native cryptographique.

## Suite et limites

G et R doivent partir du diff réellement intégré et des artefacts accessibles,
fresh en parallèle, sans exécutions/écritures de production autonomes par QA.
G propose KEEP/MODIFY/REJECT/DEFER avec oracle avant code ; lead persiste/exécute.
R indépendant examine invariants/erreurs/ownership et limites des preuves.
Puis intégration de leurs éventuelles corrections avant T hors board et F.
L photographie documentaire finale après vérification effective.
Image rasterisée,perf,ASan,Release/autreOS/API,lifetimeVBOglobal non démontrés.
ADR0003/5 Accepted/partial,0008WindowsDebug,0001/2/4/6/7Proposed inchangés.
