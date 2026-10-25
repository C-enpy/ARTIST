# 10 — Reprise autorisée : correction test/mocks bornée

2026-10-25. L'utilisateur demande d'arrêter le backend puis de reprendre la mission, après la proposition corrective du rapport09. Reprise limitée à cette proposition ; pas de changement de politique modèles ni d'élargissement Reader/F8.

## État et ownership

Backend de présentation arrêté après contrôle de la ligne de commande du PID41308 ; `REMAINING_LISTENERS=0`, handle shell terminé. Aucun service tiers arrêté. Le code1 du shell est la conséquence de l'arrêt demandé, pas un échec de la campagne.

Tâche locale indivisible, propriétaire lead pour le cycle test corrigé/runner ; pas de worker build substitué à un profil QA. Pool-1/pool-2 restent inactifs. HEAD documentaire de départ77b8550 ; sources318bf7a avant cette tranche, aucune modification de production autorisée. Ressources exclusives : fichier test et index Git ; un build/test par arbre, bail GL interprocessus pour chaque run. Aucun nouveau script exécutable, dépendance, service ou installation.

Méthode PM/LLM locale : complexité brute62, **retenue70**, risque65. Incertitude moyenne sur le chemin exact du pretty-printer ; expérience discriminante ci-dessous. Le plan17 commité est réutilisé ; cette note amende uniquement les obligations de la tranche à partir du défaut réellement observé. Aucun nouveau generator ou qa-planner demandé.

## Contrat et fichiers autorisés

But : le test `ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders` doit effectuer son chargement mocké et vérifier le même ordre detach → suppression programme → destruction shader, sans débordement dans le diagnostic de son mock de sortie. Correctif autorisé en premier lieu dans **ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp uniquement**. Si cela exige un contrat mock partagé ou une modification de production, arrêter cette tranche et argumenter la nouvelle frontière avant édition.

Invariants : assertions5/8 et attentes d'ordre/count à1 inchangées ; action ON_CALL de réflexion conservée ; aucun préremplissage NUL de production, aucune suppression globale des warnings, aucun NiceMock généralisé, aucune exclusion CTest ou option ASan affaiblie. Signature de glGetActiveAttrib et Reader non modifiées.

Localisation/impact : pile du rapport09, test347–373, mock512/ON_CALL416–429 ; buffer non-NUL AttributeReader70 avant appel74. `ATTRIBUTE_NAME` du mock vaut attribute_test, taille15 avec terminaison. Le test n'a pas d'EXPECT_CALL pour la réflexion d'attribut ; le mock la traite comme appel non intéressant et son printer GLchar* peut lire le buffer avant l'action. Référence exacte glGetActiveAttrib_mock trouvée dans8endroits bornés des tests/mock ; ne pas changer leurs contrats partagés.

Serena configuré à la racine ARTIST, LSP cpp ready ; find_symbol et overview du fichier de tests renvoient vide. Limite de couverture des macros GTest/symboles de cette TU, pas absence du test : localisation et impact vérifiés par lectures de lignes connues et grep exact. Aucun refactoring symbolique prétendu.

## Amendement QA : oracles avant correctif

Exigence explicite : retirer le débordement réellement observé, garder oracle lifetime/ordre et instrumentation. Exigence inférée : ne pas déplacer le problème en désactivant le logger ou en modifiant le buffer de production.

Top3 risques, échelle QA I×P :

1. Masquer une lecture hors bornes par une suppression globale des diagnostics (4×4=16). Déclencheur : logging de char* de sortie ; blast radius : toute la suite mock ; détection : comparaison default/quiet puis GREEN au réglage default ; mitigation : attente locale stricte seulement ; gap : autres appels non intéressants hors population exécutée.
2. Affaiblir l'ordre de destruction (4×3=12). Déclencheur : édition du test lifetime ; blast radius : contrat detach/program/shader ; détection : diff + test existant et revuefresh ; mitigation : assertions et InSequence intacts ; gap : preuve de sensibilité à une régression d'ordre à examiner par reviewer.
3. Mélanger les preuves H3/H4 (4×3=12). Déclencheur : mutation du TU test ; blast radius : preuve normale et ASan CPU ; détection : nouveaux hashes/manifeste et listes identiques ; mitigation : conserver archive rouge et nouvelle archive après build minimal ; gap : audit test-reviewer natif distinct encore non admis.

Graphe local : contrat commité → RED ciblé/contrôle discriminant → édition minimale → build unit ASan → GREEN default → non-régression CPU concernée et campagne ASan finale avec nouveau manifeste/sélection → revue adaptée/consommateurs après intégration. La lecture du diagnostic harness peut avancer indépendamment ; pas de compilations concurrentes d'un même arbre.

RED déjà acquis : campagne555, heap-buffer-overflow dans le test nommé. Expérience nouvelle avant édition : même binaire, même test isolé, réglage GMock default puis contrôle `--gmock_verbose=error`. Un PASS du contrôle silencieux **ne vaut pas correctif ni GREEN du candidat** ; il isole seulement le rôle du diagnostic. Ne pas répéter les mêmes runs sans information. Archiver codes, sortie et instrumentation.

GREEN minimal proposé si l'expérience confirme : rendre l'appel de réflexion d'attribut explicitement attendu dans ce seul test, avec programme/index et pointeurs de sortie vérifiés, cardinalité1 ; conserver l'action nominale du mock. Vérifier le réglage GMock default sans flag de suppression et la présence des assertions de destruction.

Non-régression : unit normal/ASan affectés par la mutation ; réutiliser les preuves GL normales si binaires/sources identiques, ne pas annoncer un nouveau run normal555. Campagne finale ASan complète555 suivant le protocole13 et nouveau pin ; zéro failure/error/skip exigé. Revue indépendante adaptée avant verdict final sensible, sans se prétendre test-reviewer hors board.

Registre preuves : rapport09/archive final-20261025-01 ↔ défaut initial ; futures archives ciblées ↔ hypothèse/RED/GREEN ; nouveau manifeste ↔ campagne finale. Toute preuve non reliée garde sa portée d'origine. Budget qualitatif : setup puis75%cycle prévu/25%buffer ; pas de boucle de seeds ou de build propre inutile.

Sortie : patch minimal + preuves causales et non-régression ; sinon FAIL/INCONCLUSIVE précis et restitution au lead. Audit natif indépendant et QA-PORTABILITE restent des livrables distincts, pas automatiquement clôturés par ce correctif.

## Résultat pré-édition et ajustement discriminant

Même binaire ASan, test filtré : **exit86 et même heap-buffer-overflow** (`out/logs/h4-focused-red-20261025-01.log`). Contrôle CLI quiet : également86 (`h4-focused-quiet-control-20261025-01.log`), donc pas de PASS de contrôle. Lecture de tests/src/main.cpp : seul InitGoogleTest est appelé ; les macros du gmock-port.h lié initialisent directement les flags, sans reprise environnement. Le flag CLI GoogleMock n'est donc pas un contrôle calibré dans ce runner. Aucun changement de main/flag pour réparer artificiellement l'expérience.

La nouvelle expérience reste dans le contrat : le diff minimal de l'attente locale est la seule variable, les actions ON_CALL et diagnostics par défaut restant identiques. Le RED valide avant édition est acquis ; GREEN avec ce seul delta vérifiera le chemin intéressant/non intéressant et l'exécution de la même action. La preuve n'utilisera jamais un flag silencieux. Cette note remplace la condition « contrôle quiet0 » par cette comparaison source minimale ; pas de répétition du contrôle CLI invalide ni de nouveau périmètre.
