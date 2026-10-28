# Entrées neutres pour audit final indépendant

## Contrat et candidat

Objectif : parité QA Windows / Ubuntu WSL / Ubuntu CI, normal et ASan, en réutilisant Conan/CMake/CTest et fixtures existants. Pas de performance, Release, UBSan, GPU matériel Linux, absence exhaustive de fuites ou garantie universelle OpenGL 3.3.

Candidat intégré `1532b78d96489923773e7b2a8b355dc91f94345c`. Runtime/config moteur identiques à `78e7b74989dd463cddf14080916e03b60405640d`, sur lequel portent les six campagnes moteur. Les commits suivants ajoutent seulement des documents et trois tests Python du contrôleur ; aucune preuve moteur n'est rebaptisée exécution du commit suivant.

Plan : `.opencode/evidence/mission-linux-qa/qa-plan.md` (O1–O8 et complément Attribute). Contrat Attribute : `attribute-correction-contract.md` dans le même dossier. Gates : version core 4.3 OU annonce effective d'extension, puis trois pointeurs obligatoires. Noms publics legacy exacts ; nom moderne temporaire pour classification. Réponse moderne invalide : échec du staging et génération précédente intacte. Pas de nouvelle garantie de rollback CPU du wrapper.

## Diff et sources locaux

- Diff intégré global du portage : `out/logs/mission-linux-qa/linux-qa-integrated.diff`, historique jusqu'à 0a4a671.
- Correction ultérieure : `gate-repair-integrated.diff` et `gate-repair-integrated-identity.json`.
- Derniers tests du contrôleur : `qa-gaps-integrated.diff`.
- Sources : `scripts/linux-evidence.py`, `scripts/build-linux.py`, `scripts/verify_asan.py`, `scripts/tests/test_linux_evidence.py`, `scripts/tests/test_build_linux.py`, tests du vérificateur, `cmake/AddressSanitizer.cmake`, CMakePresets et workflow existants.
- Attribute : Reader, mock glFunctionMock, tests Reader et composant Attribute aux chemins habituels ARTIST.
- Ne pas lire les rapports narratifs auteurs, leurs transcripts ou le board d'implémentation pour justifier cet audit. Le diff préexistant profils/harness/docs est exclu et à préserver ; ne pas l'assimiler au candidat exécuté.

## Preuves primaires et runner

Toutes les campagnes ci-dessous ont été exécutées par le lead ou le runner GitHub indiqué, pas par le reviewer final. Racine des preuves : `out/logs/mission-linux-qa/`.

- Windows lead : `gate-green-windows-{normal,asan}.xml`, découvertes `-discovery.json`, commandes/logs correspondants ; résumé `gate-green-windows-summary.json`, sources `gate-green-identity.json`.
- WSL lead : `gate-final-linux/20261028T201504Z-8aba0173a35247e4a58c624787680584` et `20261028T201850Z-5bd88de1cef04e518c968dd062b0827a`.
- GitHub runner Ubuntu : `ci-40760506988/`, deux jobs normal/ASan au HEAD 78e7b74. Chaque sous-dossier linux-evidence contient HEAD, delta suivi, manifeste d'identité, découverte et XML distincts unit/opengl.
- Recoupement lead : `gate-final-reconciliation.json` ; chacun des six runs : 589 cas = 513 CPU + 76 GL, zéro failure/error/skip. Quatre ensembles Linux strictement identiques, bijection Windows/Linux après le seul cast `(glm::qualifier)0` → `0`, sans collision.
- 149 hashes source/config par run Linux vérifiés ; deux fichiers Windows diffèrent par CRLF seulement, leurs blobs Git exacts correspondent aux hashes Linux. Manifestes et delta suivi vérifiés.
- ASan WSL : `gate-final-linux-instrumentation.json`, commandes de compilation de 43 TU unit et 13 TU composants instrumentées, symboles ASan et libasan dans les exécutables, hashes binaires exécutés concordants. CI : compile_commands.json uploadés, résumé `gate-final-ci-instrumentation-canaries.json`.
- Canaris WSL et CI : summary.json et journaux sous canary, sain 0, use-after-free 86, diagnostic ASan, options canoniques et detect_leaks=0.
- Contrôleur : `qa-gaps-{windows,linux}.log`, 9 tests exécutés dans chaque OS par le lead. Tests de fixtures uniquement ; pas preuve GPU. `qa-gaps-mutations.json` : trois mutants en mémoire détectés, aucune source production mutée sur disque. Test SHA256 `677def32a33472e51879811e79866b327c4e54948b3700279ed4a25875b4f502`.

## Échecs et incertitudes conservés

Les campagnes Linux/CI initiales avaient trois échecs Attribute et restent archivées. Le premier RED de classification est causalement observé mais son delta complet n'a pas été hashé à chaud : limite historique, aucun hash final ne lui est attribué rétroactivement. Le nouveau RED du gate GLEW est, lui, identifié à chaud : `gate-red.patch`, `gate-red-identity.json`, `gate-red-windows.xml`, 513 CPU dont un échec attendu avant production.

Le premier passage Windows intégré a subi des violations d'accès ; deux CPP seulement avaient été recompilés après changement de headers. À sources inchangées, clean puis 63 CPP reconstruits donne 588/588. Logs/XML des deux expériences conservés ; cause exacte du suivi de dépendances non entièrement expliquée. Les campagnes finales citées sont reconstruites proprement.

## Mandat d'audit proposé

Lecture seule, contexte fresh indépendant, hors board. Vérifier obligation → preuves et test → obligation, nominal/négatif, mutant mental discriminant avant finding fort. Ne pas exécuter, écrire, déléguer ou demander une commande interdite ; le lead fournit les digestions ci-dessus et persiste la remise texte. Distinguer bugs bloquants, suggestions et limites hors périmètre. Ne pas fabriquer de finding si les tests sont discriminants ; ne pas déduire l'exécution d'un nom de fichier.

**Admission non encore réalisée** : la vérification des permissions effectives et du canal de restitution reste à qualifier ; une consultation native de profils a été refusée auparavant et n'est pas répétée sans autorisation distincte. Ce document ne vaut pas admission ni audit.
