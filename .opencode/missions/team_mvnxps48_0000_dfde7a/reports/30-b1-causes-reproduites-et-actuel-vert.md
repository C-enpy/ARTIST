# 30 — B1 : deux causes reproduites, candidat actuel conforme aux réductions

2026-10-25. Runner lead, protocole29 figé avant exécution. Aucun correctif/source/build nouveau, aucun balayage de seeds. Proposition pool-5 statique puis expériences réellement exécutées par le lead ; pas de confusion d'attribution.

## Résultats exécutés

Archive out/qa-b1/causal-20261025-01/ : manifeste sélection pré-run, run-start/end, arguments/cwd/horodatages et exit par bras, stdout/stderr complets, quatre XML, verification-summary.json. Processus séparés, GTEST_* retirés, shuffle0/repeat2/colorno, CWD racine canonique. Identités H/N avant/après concordantes au29. Exécution18:25:49Z, aucun timeout/crash.

- **A historique : exit1**,4RUN/2cas uniques, un échec causal au second callCreate. glGetShaderiv utilise encore la default de shader/LoaderTests.cpp128 (stdout81), puis COMPILATION_FAILED ; le test Failure précédent a contaminé le singleton. XML dernière itération2cas/1failure/0skip.
- **A actuel : exit0**,4RUN/2cas uniques, zéro échec/skip, XML dernière itération2cas/0failure. Aucune sortie stdout non vide après dernier PASSED ; stderr contient exactement les deux diagnostics de compilation volontairement échouée attendus dans Failure, pas un échec GPU réel.
- **B historique : exit1**,6RUN/3cas uniques, deux échecs au second tour : Loader passID5 au lieu de1, ShaderLoader shaderID8 au lieu de1. Les defaults sont attribuées explicitement à ShaderLifetimeTests.cpp360/359 (stdout159/187), writer exécuté au premier tour. XML dernière itération3cas/2failures/0skip.
- **B actuel : exit0**,6RUN/3cas uniques, zéro échec/skip, XML dernière itération3cas/0failure, stderr vide et aucun texte stdout après dernier PASSED. Le writer charge/détruit sans échec préalable de fixture.

Ces deux contre-exemples négatifs et deux nominaux satisfont les oracles figés. Les synthèses FAILED après PASSED de H sont les échecs attendus et conservés, pas des diagnostics tardifs ignorés. Les avertissements GMock d'appels non intéressants ne sont pas supprimés globalement ; seuls les diagnostics de défaut précis sont interprétés avec leur test/ordre. Le XML GoogleTest ne cumule pas les itérations :5cas uniques par binaire,10RUN, pas10nouveaux tests.

## Cause et disposition technique proposée

Deux contaminations de defaults ON_CALL du singleton mock processus sont désormais **isolées par témoins causaux**, indépendamment d'une suite actuelle verte ou d'un filtre176sur484. reset VerifyAndClearExpectations n'efface pas les ON_CALL. Les trois sites actuels utilisent déjà EXPECT_CALL/WillOnce et ne contaminent plus ces observers dans les deux réductions. Aucun défaut moteur/GPU ou référence pendante établi par ces expériences.

B1 n'appelle donc **aucun nouveau correctif spéculatif** : la cause historique est identifiée et le comportement actuel vérifié au périmètre des deux mécanismes. À recouper en revue finale de la mission avant clôture ; aucune garantie de toutes permutations ou de tous mocks. Les primaires historiques176 et leurs FAIL restent intacts. Aucun replay de leurs deux seeds n'est nécessaire pour obtenir la nouvelle information discriminante désormais acquise.

Source actuelle82bf52d ; H3d4fb31e…6433280octets et N1b0fc029…11142144octets, SHA complets au29/manifeste. Logs stdout SHA256 : A-H8a6636c5… ; A-N1334a26d… ; B-H02ac6945… ; B-Ne9c58910…. Pas de campagne ASan/GPU/image/performance/Release/GCC/Linux, pas de reprise automatique d'un ancien verdict sur un nouveau diff.

Les enquêtes B3 et B5 continuent indépendamment. B3 attend son contrat adopté et l'amendement QA avant auteurs ; B5 doit encore statuer explicitement sur les nouvelles preuves natives. Aucune dette de ces branches fermée par le succès B1.
