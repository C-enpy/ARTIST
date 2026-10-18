# PM2 — graphe du lot Attribute-GL renforcé

Auteur proposition : project-manager / pool-5, tâche task_mvdwtm9f_000j_p72czk,
contexte fresh, lecture seule, résultat natif reçu et relu par le lead. Aucun
code/build/test ni publication par PM2. Pool-5 arrêté avant intégration texte.
Base intégrée ece666c, delta moteur depuis 4a9b722 vide sur ARTIST/scripts/cmake/
CMakeLists.txt/CMakePresets.json/conanfile.txt. Diff externe harness/profils,
AGENTS.md et .grepai/config.yaml préservé et exclu des futures écritures moteur.

Méthode PM/LLM, pas JEV. Complexity/risk entiers indépendants, pas probabilités.
Politique mission approuvée v2 inchangée ; le champ JSON version1 est le schéma,
pas la révision d'approbation. Aucun modèle explicite ni manipulation de scores.
Q2 indépendant en cours, non lu par PM2 ; ne pas prétendre avoir intégré son plan.

## Scores proposés et validés par le lead

- D, gpu-engineer : complexité brute82/retenue82, risque78.
  Ancien78/76 ; +4 pour TF interleaved120octets, separate, 12sorties exactes,
  sondes nom/array, taxonomie des preuves et mock IPointer dans un cycle TDD.
  Risque+2 : synchronisation/readback de fixture, arrêt avant draw invalide,
  spelling driver non résolu. Injections et gardes améliorent la détectabilité,
  mais ne justifient pas le risque64 suggéré par A2 sans argument suffisant.
- G, test-engineer : brute64/retenue70, risque58. Plancher développement appliqué
  aux propositions de tests exécutables malgré persistance/exécution par le lead.
  +10 brut pour matrice TF et classification native/injection, risque de faux PASS.
- R, correctness-reviewer :66/66, risque68. Revue pas développement ; manquer une
  conversion float32 ou un draw invalide justifie le passage65->68 assumé.
- T, test-reviewer :56/56, risque63. Fresh hors board ; oracles mal classés et
  ancien40/40 sont les principaux pièges d'audit.
- F, rendering-verifier :61/61, risque71. La clôture engage TF exacte et limites
  spelling/nom, pas recyclage de299/299 ou40/40.
- L, documentation bornée :32/32, risque27. Transcription/réindexation après F,
  sans promotion non prouvée des statuts ADR.

Avec la grille approuvée, routage attendu (pas override) : D Sol xhigh,
G Sol medium, R et F Sol high, T Grok Extra High via Cursor, L Luna none.
Éléments incertains : spelling glGetActiveAttrib pour array[1], nom300/256 lié,
12sorties linkées, alias GLM, purge GLEW unique. Développement>=70 conservé.

## Budget proposé, à recouper avec Q2

Setup36 = limites6 + sondes10 + noms6 + mocks/RAII14.
Planifié90 = layouts4 + exclus3 + Reader8 + nom6 + array8 + mat4 4 + Setter10
+ erreurs8 + leurre6 + TFinterleaved14 + separate8 + injections6 + nonrégression5.
Buffer27, soit total153. Après setup :90/117=76.9%,27/117=23.1%.
L'ancien total140 sous-comptait10 ; le garder avec planifié90 laisserait
buffer14, ratio86.5/13.5 non conforme. Alternative150=36+90+24 ->78.9/21.1.
Lead retient153 comme proposition de référence, attend convergence Q2.
Noms6 de setup (préflight link) et nom6 planifié (oracle Reader) doivent rester
distincts ; si doublon réel, corriger le calcul, pas couper TF/injections haut risque.

## D : un auteur de bout en bout

Entrée obligatoire : Q2 amendé réellement persisté et commité avant worktree.
Ne pas couper le worktree d'un arbre sale ou supposer ses diffs non committés reçus.
Fresh, worktree isolé, moteur+tests de tranche possédés par gpu-engineer.
Écritures bornées : AttributeReader.hpp, attribute/Setter.hpp,
opengl/context/AttributeContext.hpp, AttributeConcept.hpp si trait fermé requis,
tests/mock/opengl/glFunctionMock.hpp pour IPointer, tests Attribute contrats/
Reader/Setter/contexte/ownership, AttributeComponentTests.cpp,
AttributeReadingTest dans component-tests/src/graphic/shader/PassTests.cpp,
shaders sous test-datas/shaders/qa-attribute/. Pas d'édition Binder/Unbinder,
pas de hook TF dans Pass::load ni modification du harness global.

Traits/layout/Pointer-IPointer compile-time ; réflexion après link GLSL runtime ;
VAO/VBO/payload runtime au set. Pas cook ni nouvelle obligation link-time C++.
Validation avant tout GL : buffer0, valeur absente, array/size, enum, usage.
Refus sans BindBuffer/BufferData/Pointer/IPointer. Succès lie sa propre cible,
binding reste sa cible, leurre intact. Freeze/shared_ptr/ownership/emprunts inchangés.

TDD, setup et critères détaillés : attribute-contract.md et attribute-oracles-a2.md.
RED causal avant production sur outillage réel, pas mock manquant/contexte absent.
Entiers INTEGER faux => arrêt avant TF, pas comportement shader indéfini utilisé
comme échec. Arrays330 attendent compile/link succès, pas refus du langage.
Nom long tronqué si shader naturel lié ; flag array faux seulement si [0] observé.
Injections CPU étiquetées, jamais preuve driver. Float scalaire RED via leurre.
GREEN minimal : 12layouts/is_same ; mapping/refus ; TF120octets typée et
sentinelle0xA5/une primitive ; separate float/int/uint/vec4 ; Reader noms longs/
arrays/mat4 et aPos ; cleanup fixtures avant glfwTerminate. Aucun delete production.
Refactor seulement après GREEN, puis non-régression intégrée et preuve candidat.

Commandes présentes, vérifiées mais non exécutées par PM2 : scripts/build-windows.ps1
-Configuration Debug -TestLabel unit -Jobs 1 puis label opengl, ou CTest direct
--test-dir out/build/windows-msvc-debug -L unit|opengl --output-on-failure
--no-tests=error --parallel 1. Utiliser l'espacement réel des paramètres.
Un compilateur par sortie, bail contexte GL exclusif, RESOURCE_LOCK artist_opengl.
Les commandes exactes du worktree seront adaptées à son chemin sans réutiliser
un cache CMake attaché à une autre racine. Runner QA = lead, pas un agent autonome.

## Consommateurs, barrières et parallélisme

G après shutdown/merge D, diff inspecté et preuves réellement accessibles :
propositions KEEP/MODIFY/REJECT/DEFER, jamais production ; lead persiste/exécute.
R fresh en parallèle de G, lecture indépendante du même candidat ; toute correction
production invalide la partie affectée et appelle ré-audit.
Intégrer propositions G/corrections R avant T. T fresh hors board d'implémentation,
scénario discriminant avant finding haut ; correction par propriétaire puis ré-audit.
F peut préparer sa lecture pendant T, mais verdict après T et non-régression finale.
Pas de bail GPU concurrent des runs lead. Toute case native non exécutée reste
INCONCLUSIVE ; aucun PASS ASan/image/perf/Release/autreAPI.
L après F : next-mission, current-state, correction-plan, log et preuves ADR,
réindexation artist-wiki sérialisée. Ne pas confondre rejet support arrays dans
Setter (contrat conservé) et légalité des déclarations arrays GLSL (corrigée).

Graphe : A2 intégré -> (Q2 || PM2) -> commitQA/amendements et décisions nécessaires
-> D -> shutdown/merge/diff -> (G || R) -> intégration -> (T || préparationF)
-> verdictF -> L. Un propriétaire pour RED/GREEN/REFACTOR, pas trois tâches.
Tous les consommateurs D/G/R/T/F/L restent non publiés jusqu'à intégration des
entrées ; IDs réels et HEAD/diff fournis à publication. Pas de tâche factice barrière.

## Limites

40/40 anciens binaires (36CPU,4GL) prouvent seulement le préflight exécuté par
le lead. Ni compilation récente ni nouveau contrat. Recommandations PM2 ne
résolvent pas automatiquement les décisions humaines sur preuves impossibles
ou spelling driver ; Q2 doit les expliciter. ADR0003/5 Accepted/partial,
0008 vérifiéWindowsDebug,0001/2/4/6/7 Proposed, pas ADR0009.
