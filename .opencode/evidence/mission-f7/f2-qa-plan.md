# Plan QA F2 — proposition f2-qa-plan, validée par le lead

2026-10-18. Auteur proposition : `f2-qa-plan` (lecture seule, zéro exécution).
Persistance et runner : lead. Contrats : `f2-correction-contract.md` et plan
initial `.opencode/evidence/mission-gpu-qa/f7-qa-plan.md`.
Ce commit précède tout worktree auteur F2.

## Pin et préconditions

Pin observé main `483360e` par le planner ; code cible à identifier avant auteur
par HEAD/delta/SHA256. ADR0001/2/4/6/7 Proposed ; 0003/5 Accepted partial ;
0008 Windows Debug seulement. Aucun ADR adopté, aucune API array ajoutée.
Le delta récent T1-1 et les documents de clôture ne doivent pas être assimilés
à un changement des setters. Si les cibles F2 ont changé, re-préparer ce plan.

## Exigences explicites et inférées

- S1 : garde `getGLSize()!=1`, exception runtime_error au motif
  `ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED`, taille dans le diagnostic, avant GL.
  Nominal count1 ; négatifs size2 et size0, exception + zéro appel GL.
- S2 : count littéral1 aux 30 appels (inventaire à confirmer), 34 spécialisations
  dont quatre scalaires. Exécution des familles mockées ; line-map complète pour
  les autres, tag static-presence distinct d'une preuve driver.
- S3 : init déterministe size1/id0/type0, construction directe et make_unique.
  RED causal size via make_unique actuellement0 ; id/type déjà0 dans ce chemin :
  pas de faux RED. Ne pas lire volontairement des champs indéterminés avant fix.
- S4 : Reader conserve noms et tailles arrays. Tests existants réflexion size2 et
  publication partielle size2/3 restent inchangés. GL après GREEN : tableau vec3
  de deux éléments tous actifs, rejet avant upload, référence avant appel et
  readback après, aucune altération. Nominal scalar/vector/matrix single-value.
- S5 : stockage avant dispatch conservé, aucun rollback promis. Pas de test
  imposant que std::any reste inchangé après rejet ; GAP accepté par contrat.
- I1 : UNSUPPORTED_TYPE/concepts inchangés, tests existants/static-presence.
- I2 : garde dans on(), après stockage du wrapper mais avant GL.
- I3 : diagnostic avec taille, oracle au motif, pas texte intégral rigide.

## RED sûr puis GREEN

A1 : make_unique + vec, count1 attendu contre count0 simulé actuel.
A2 : size2, throw attendu et Times(0), appel mock observé aujourd'hui.
A3 : size0, throw attendu, absence de throw actuelle.
A4 : scalar size2, throw + Times(0), appel scalar actuel.
A5 : make_unique getGLSize()==1 contre0 actuel.
Premiers FAIL conservés avant modification production et avant adaptation des six
oracles historiques count0 dans UniformTests. Aucun driver OOB volontaire.
Garde + count1 + init seulement, puis non-régression.

## Registre preuves/oracles

UniformTests : dix tests historiques (rectification du « neuf » de la proposition),
dont six count0 à renforcer après RED et quatre scalaires. UniformContractTests :
inventaire34types à recouper. UniformValue/Ownership/T1-1 inchangés.
UniformReaderTests et composants Reader non modifiés. Nouveaux tests F2 et fixtures
à créer par auteur, re-glob obligatoire. glFunctionMock : extension non attendue ;
si nécessaire, oracle précis et justification, jamais refonte massive.
Readback GL : interroger CHAQUE élément par sa location glGetUniformLocation
propre ; ne pas supposer que glGetUniformfv(location[0]) copie tout le tableau.
Doubles/fp64 gated : capacités observées ou INCONCLUSIVE driver ; aucun mock ou
inventaire statique requalifié en preuve native. ASan non requis pour ce lot
(pas ownership nouveau) et ne démontre pas la sûreté d'un accès driver.

## Risques I×P

- Oracle réécrit sans causalité : 4×4=16. Déclencheur adaptation count0 ; portée
  chaîne de preuve ; détection RED daté/revue ; mitigation conserver FAIL et
  Times exacts ; gap enforcement technique absent.
- OOB driver : 5×3=15. Déclencheur arrayN>1 avec buffer unique ; portée setters ;
  détection faible ancienne suite count0 ; mitigation garde + count1 + GL après
  GREEN ; gap ASan ne couvre pas le driver.
- Familles non mockées/doubles : 3×5=15. Déclencheur site oublié ; portée familles
  de types ; détection inventaire/revue ; mitigation34/30 + GL3.3 représentatif ;
  gap natif non exécuté explicitement INCONCLUSIVE.
- Delta candidat : 4×3=12. Détection HEAD/SHA avant-après ; mitigation barrière
  intégration/requalification si delta source ; pas de preuve périmée tolérée.

## Budget et décisions lead

Budget100 : setup25, planifié56 (75% du reste), buffer19 pour driver/re-audit/
re-glob/capacités. Couper un cas = gap nommé avec owner/impact, pas PASS implicite.
DA : garde on() retenue. DB : GL3.3 nominal scalar/vector/matrix + rejet array
et readback tous éléments ; pas d'image/draw requis. DC : doubles seulement si
capacité observée ; sinon limite driver, revue de tous les sites obligatoire.
Évaluation lead implémentation PM/LLM brute68/retenue70, risque60 indépendant.

## Sortie et ressources

G1 identité code avant auteur ; S1–S5/I1–I3 taggés executed/static/inference.
RED A1–A5 conservés ; inventaire34/30 oublié0 ; commandes/JUnit frais sortie65536,
comptages non nuls, zéro skip inexpliqué, re-glob après nouveaux fichiers.
Logs sous out/mission-f7/f2 ; baux build/cache/GL exclusifs au seul auteur,
build un job ; source worktree isolée et sortie courte f2-build si besoin.
Fichiers autorisés : contexte OpenGL Uniform, setters Uniform, tests CPU/GL et
fixtures dédiées. Reader/Pass/Attribute/T1-1 hors lot. Pas de changement global.
Plan commité avant auteur, puis intégration lead avant revue fresh indépendante.
F1 diagnostique séparé. Aucun verdict global, performance ou UB exhaustive.
