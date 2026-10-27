# Entrées neutres — audit fresh des tests du float préparé

## Objectif, contexte et identité

Audit en lecture seule de la capacité des tests à détecter des erreurs plausibles
dans le contrat adopté. Aucun test, shell, edit, délégation ou réparation de harness.
Contexte ARTIST canonique explicite ; ancien pool-5 non requis. Lire ce fichier
et un test relatif puis restituer par réponse finale native. Aucun board, autre
reviewer, rapport de justification auteur ou proposition generator à consulter.

Source finale : `3d0e1452cd9bd7b126b5fd1e43c73a5d0b1b6e23`. H2 précédent
`c283f806f93167b86ab0278d58714764873022ca`. Depuis H2 : deux scénarios CPU et
trois assertions de miroir ajoutés dans PreparedFloatUniformTests ; les huit
headers de production et les consommateurs graphiques sont inchangés. Les commits
suivants éventuels de cette mission ne portent que documentation/preuves ; le lead
fournit le HEAD d'admission et son delta exact dans le brief.

Contrat adopté et QA antérieure au code : `contract.md`, `qa-plan.md` ici.
ADR-0004 globalement Proposed, seule tranche float adoptée ; ADR0003/5 partial,
ADR0008 Windows Debug uniquement. Pas de performance, zéro allocation, autre API
ou RAII exhaustive à prouver dans cette mission.

## Obligations observables à auditer

1. Float exact scalaire préparé après link/réflexion runtime ; arrays[1], bool,
   absents/optimisés, noms invalides, métadonnées fabriquées/altérées refusés.
2. Témoins/générations AVANT pointees, contexte/programme avant miroir/upload ;
   owners uniques conservés, mono-thread/non réentrant, pas de relink/delete GL
   externe et fenêtre vivante jusqu'au retrait. Metadata/maps/free/destruction,
   moves/destination chargée et ID numérique réutilisé ne réhabilitent pas l'ancien.
3. Rejet avant publication conserve l'ancienne identité vivante ; candidat complet
   avant swap/estampille puis retrait ancien ; pas de rollback GL global promis.
4. Updates répétés : trois uploads exacts, pas de lookup CPU, résolution native ou
   any_cast. Miroir CPU historique et API par nom restent cohérents.
5. Consommateur target préparé une fois, decoy par nom ; mismatch puis récupération
   contrôlés par miroir et glGetUniformfv des deux programmes.
6. Pixel : oracle fixe indépendant non vide pour chaque route à 0/.5, puis égalité
   des buffers. FBO RGBA8 linéaire32×32 et états explicites. (8,8) blanc→fond,
   (24,24) fond→blanc, (18,16) blanc commun, (2,2) fond commun.
7. Preuves séparées RED/GREEN/expériences/final, découvertes/sélections/noms/skips,
   candidat/source/binaire et ASan canari ; absence de preuve n'est pas PASS.

## Chemins locaux

- `ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp` et contexts
  communs/OpenGL PassContext/UniformContext/PreparedUniformState, composants pass
  Freer et UniformReader sous ARTIST/include/graphic/.
- `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp` (20 tests).
- `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp`.
- `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformRenderTests.cpp`.
- `ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp`.
- Fixtures UniformReaderTests et ShaderLifetimeTests, helpers/mock dans
  `ARTIST/tests/mock/opengl/glFunctionMock.hpp` et fixture GL
  `ARTIST/component-tests/include/OpenGLComponentTests.hpp` si nécessaire.

## Données de preuve neutres, exécutions runner lead

Sous `out/qa-prepared-float/lead-runner-20261027/` :
- `final-validation.json`, SHA256
  `312642425d7fd039a0469a15be373fde7a55f651b3a6bf5b7b6a0b2261eb048c` : identité,
  quinze empreintes source, cinq binaires/probe, commandes et liste de noms.
- `10-final-build-unit.command.json` et log : commande convenue Debug/unit exit0.
- `11-final-discovery` JSON commande/log : CTest -N exit0, 579.
- `12-final-discovery-json.log` : 579 noms uniques.
- `13-final-unit.junit.xml` : 504 uniques, zéro failure/error/skip.
- `14-final-opengl.junit.xml` : 75 uniques, zéro failure/error/skip.
- `17-final-asan.junit.xml` : 579 uniques, mêmes noms que union normale,
  zéro failure/error/skip. Commande convenue AddressSanitizer exit0.
- `18-final-asan-discovery.json` : mêmes 579 noms.
- `final-asan-preflight/summary.json` : healthy0, use-after-free86, PASS,
  options canoniques halt_on_error1/abort_on_error0/exitcode86/detect_leaks0.
- `09-normal-clean-build` et `16-asan-clean-build` commandes/résumés : clean-first
  complet, un job, exit0, 69 étapes chacun. Logs complets sous out/logs/ avec
  préfixes prepared-float-lead-normal-clean et prepared-float-lead-asan-clean.

Les XML, noms et empreintes ont été recoupés par le lead ; l'auditeur n'a pas à
exécuter ou recalculer les gros artefacts. Il peut examiner les extraits/scénarios
nécessaires avec read/grep ; cette digestion ne prescrit aucune conclusion.

## Expériences séparées et exclusions matérielles

Nominal pré-expériences `05-nominal-prepared` : 20 passants.
Trois mutants séparés dans set, builds réussis et chacun un test failure/CTest8 :
- `06-mutant-C` : véritable getUniform("scalar") via pass temporairement retenu,
  compteur CPU10 au lieu4, soit delta6 ; trois uploads, extraction inchangée.
- `07b-mutant-L-recompiled` : appel glGetUniformLocation supplémentaire, Times(0)
  dépassé trois fois, C/A inchangés.
- `08-mutant-A` : getValue<GLfloat> supplémentaire, extractions4 au lieu1,
  soit delta3 ; C inchangé, trois uploads.
Diff, commandes, JUnit et hashes disponibles par préfixe. Source restaurée et
empreinte contrôlée après chaque mutant ; header final SHA256
`a791eb6bb0ebd092112e3557a00a41416d59baf3df48455d738e9650e2b1c02d`.

EXCLUSIONS : `07-mutant-L` initial réutilisait le binaire C (Ninja no work,
EXE identique) et n'est PAS preuve L. ninja -t deps sur cet objet donnait zéro
dépendance ; recompile forcée de l'objet puis clean-first final des deux arbres,
sans correction de toolchain. `01-build-unit` a échoué à compiler le patch de
test (accesseur protected), aucun test nouveau exécuté ; XML STALE-NOT-EXECUTED
exclu. Une correction test-only a utilisé le témoin public ; pas visibilité
production ouverte. Ces échecs ne sont pas des RED comportementaux du moteur.

Autres contrôles auteur disponibles localement sous
`out/qa-prepared-float/460fe52-consumer-delivery/` : `06-red-pixels.junit.xml`
(update omis, un échec), `10-black-control.junit.xml` (draw omis, un échec,
intérieur18,16 noir), puis `15-final-focused.junit.xml` restauré3passants.
Les attendus pixel avaient été fixés avant code ; égalité seule ne suffit pas.

Limites historiques : certains tests lifetime ajoutés après les gardes ; empreinte
source RED initiale manquante. Ces mutants ne reconstruisent pas ce passé.
Fuites désactivées, dépendances/driver non instrumentés, Linux/Release/autres API,
scène produit et performance non exécutés. Aucun PASS global proposé.

## Restitution attendue

Admission réelle/profile/lecture/restitution à observer, pas sandbox universelle.
Un refus donne une restitution unique bornée, pas un nouvel essai ou grant.
Audit fresh hors board : lire assertions et proposer un scénario ou mutant
discriminant AVANT tout finding haut. Distinguer défaut bloquant, suggestion
non bloquante et gap hors contrat ; KEEP/MODIFY/REJECT/DEFER, preuve/scénario,
fichier:lignes et périmètre/limites. Ne pas modifier/exécuter quoi que ce soit.
