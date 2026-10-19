# Attribute-GL — préparation, exécution non approuvée

## Candidat et méthode

HEAD inspecté : `4a9b722a8f1b64470172686aee961221fa6384c0`.
Diff préexistant : `.grepai/config.yaml`, `docs/architecture/correction-plan.md`,
`docs/architecture/next-mission.md`, `docs/index.md`, `docs/log.md` ; fichiers
non suivis de cadrage et répertoire de mission. Ne pas écraser ces changements.
La configuration GrepAI est hors périmètre. Aucun changement moteur observé.

Sources : `AGENTS.md`, `docs/engineering-principles.md`, registre ADR,
plan de correction et `docs/architecture/next-mission.md`, également retrouvé
par QMD artist-wiki (#6f3b43). Préparation PM/architecte déjà intégrée dans ce
dernier fichier ; scores revalidés localement par le coordinateur, sans nouveau
worker. Méthode PM/LLM, pas d'appel JEV. La classification configurée dans le
harness n'est pas modifiée ; toute publication utilisera les deux scores manuels.

## Objectif et frontières

Une chaîne : programme GLSL lié -> réflexion -> description validée -> upload et
configuration Attribute -> consommation native contrôlée. Famille fermée :
float, int32, uint32, chacun scalaire ou vecteur de 2 à 4 composantes.

Séparer type/cardinalité GLSL et composantes/type scalaire/stride mémoire CPU.
Float : Pointer sans normalisation ; entiers : IPointer. Refuser mismatch,
matrices, arrays (même cardinalité active 1), doubles et layouts non prouvés.
Location signée vérifiée et noms exacts sans publication tronquée. Refus avant
upload/configuration GL ; pas de rollback CPU implicitement promis. `set` lie
explicitement son VBO vivant, même après liaison d'un leurre. Après succès,
GL_ARRAY_BUFFER désigne la cible. Contexte et VAO valides sont des préconditions.
État initial déterministe, ownership/emprunts F7 et freeze conservés.

Compile-time : traits/layout et choix Pointer/IPointer. Préparation runtime
après link GLSL : réflexion et compatibilité. Runtime : VAO/VBO et payload.
Pas de nouvelle obligation link-time C++, ni de format cook.

Exclus : lifetime/destructeur VBO, RAII globale, handles générationnels, relink
atomique, nouveaux backends, matrices/arrays/doubles supportés, optimisation,
benchmark, image de référence, Release, multi-OS et réparation générale QA.
L'investigation VBO optionnelle du cadrage reste différée hors mission.

ADR à transmettre : 0003 et 0005 Accepted/partial ; 0008 vérifié Windows Debug
seulement ; 0001/0002/0004/0006/0007 Proposed. Pas d'adoption globale d'ADR-0004.

## Politique proposée persistée, non approuvée

Fichier faisant foi : `model-policy.json` adjacent. Providers autorisés conservés.
Bandes complexité et risque : 0–33, 34–66, 67–100. Risque en lignes :

- 0–33 : light, standard, grok.
- 34–66 : standard, grok, sol-medium.
- 67–100 : sol-medium, sol-high, sol-xhigh.

Groupes, chacun avec un seul candidat, sans retry implicite :
- light : opencode-go/gpt-6-luna, none.
- standard : opencode-go/glm-5.3-flash, max.
- grok : opencode-go/grok-4.7, high.
- sol-medium : openai/gpt-6.1-sol, medium.
- sol-high : openai/gpt-6.1-sol, high.
- sol-xhigh : openai/gpt-6.1-sol, xhigh.

Identités/variantes découvertes au catalogue natif. Opus retiré. GLM priorisé
dans les cases simples ; Grok puis Sol pour exigences croissantes. La grille
ne réduit pas les scores pour favoriser un modèle. Elle ne sait pas router par
mot-clé « documentation » : la tâche documentaire bornée L relève de light par
ses scores réels. Une future documentation à risque élevé exigerait révision.

## Tâches proposées (repères locaux, pas IDs Ensemble)

Tous les contextes fresh ; revues indépendantes sans justifications d'auteur.
Complexité brute -> retenue et risque sont distincts. Développement : plancher 70.

- A : figer alias GLM exacts/layouts et diagnostics publics, sans rouvrir le
  périmètre. engine-architect, 58 -> 58, risque 62, Grok high. Lecture seule,
  sans worktree. Entrées : cadrage et points d'entrée listés ci-dessous.
  Sortie : contrat explicite utilisable par QA et auteur. Risque : compatibilité
  de l'interface publique ; incertitude sur fixtures et détection d'arrays.
- Q après A : plan QA, qa-planner, 46 -> 46, risque 52, Grok high. Texte/patch
  seulement, sans worktree. Nominal/négatif, exigences explicites/inférées,
  invariants, top3 risques, traçabilité bidirectionnelle, setup, 70–80 % planifié
  et 20–30 % buffer. I×P >= 10 : déclencheur, blast radius, détection, mitigation,
  gap. Lead persiste et commit contrat/QA avant tout worktree auteur.
- D après Q intégré/commité : auteur vertical gpu-engineer, 78 -> 78, risque 76,
  Sol xhigh. Worktree isolé, unique écrivain production et tests de cette tranche.
  Ownership : AttributeReader.hpp, attribute/Setter.hpp, AttributeContext.hpp,
  traits/concepts directement requis et tests Attribute CPU/GL correspondants.
  RED causal -> GREEN minimal -> REFACTOR/non-régression. Risque élevé : ABI
  CPU/GLSL, 12 formes, voie entière et mutations driver ; une seule tranche
  couplée, pas de phases TDD réparties entre auteurs.
- G après shutdown/merge et inspection de D : test-engineer, 54 -> 70, risque
  55, Sol medium. Propositions de tests de gaps, jamais production. Oracles avant
  code et KEEP/MODIFY/REJECT/DEFER. Lead relit, persiste et exécute. Au plus deux
  réparations healer ; bug production reste rouge. Risque : faux positifs natifs.
- R après intégration D, parallèle à G : correctness-reviewer, 57 -> 57,
  risque 65, Grok high. Lecture seule sans worktree. Lifetime/emprunts, valeurs
  initiales, validation avant effets GL, exactitude noms/types/locations/arrays.
- T après intégration G et corrections R : test-reviewer, 53 -> 53, risque 60,
  Grok high. Fresh, hors board d'implémentation. Contrat/candidat/preuves sans
  plaidoyer auteur ; scénario discriminant avant finding haut, correction par
  propriétaire puis ré-audit. Lecture seule, aucune exécution QA autonome.
- F sur candidat final : rendering-verifier, 56 -> 56, risque 64, Grok high.
  Lecture indépendante, peut préparer sa vérification avec T ; clôture seulement
  après T et non-régression finale. Aucun bail GPU concurrent. Identifier candidat,
  commandes, comptages et limites ; PASS/FAIL/INCONCLUSIVE par critère.
- L après preuves finales : documentation bornée, 28 -> 28, risque 25, Luna none.
  Ownership documentation seulement, par le lead ou un auteur documentaire
  disponible ; synchroniser next-mission, current-state, correction-plan et notes
  ADR pertinentes sans promotion de statut non démontrée. Contrôles syntaxe,
  liens et cohérence, puis réindexation artist-wiki sérialisée. Pas de faux RED.

Graphe : A -> Q -> commit lead -> D -> intégration -> (G || R) -> intégration
des propositions/corrections -> (T || préparation F) -> vérification finale F -> L.
Les tâches consommatrices restent dans ce plan jusqu'à intégration de leurs
entrées ; publication ultérieure avec vrais IDs, HEAD/diff et chemins de preuves.
Un build par sortie, un bail contexte/fixtures GPU, un écrivain par fichier.

## Preuves et préconditions

Avant auteur : restitution QA autorisée effectivement disponible, contrat et QA
commités, diff de cadrage identifié dans le worktree reçu, toolchain et contexte
GL fonctionnels. Sans canal QA : BLOCKED, sans élargir ses permissions. Les trois
QA restent read/glob/skill uniquement ; propositions et runner lead séparés.

Tests possibles après inspection de l'outillage et chargement
artist-build-validation : scripts/build-windows.ps1, cibles ArtistLib_tests et
ArtistLib_component_tests ; commandes exactes à fixer par préflight, non exécutées
ici. Aucun essai RED/GREEN pour cette mission. Les 299/299 sont historiques.

Oracles : réflexion réelle vec3 -> 3/GL_FLOAT ; signed/unsigned -> IPointer ;
12 formes couvertes individuellement CPU et GL natif ; mismatch/formes refusées
sans upload/configuration ; location invalide ; noms non tronqués ; buffer leurre
intact. Valeur effectivement consommée par shader avec readback/capture numérique,
pas seulement GL_NO_ERROR. Fixtures nettoient VAO/VBO même après ASSERT.
Conserver candidat, commandes réellement exécutées, nombres, skips et artefacts.
Environnement absent/zéro test n'est ni RED ni PASS. ASan à cadrer séparément.

Points d'entrée :
ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp ;
ARTIST/include/graphic/opengl/pipeline/component/attribute/Setter.hpp ;
ARTIST/include/graphic/opengl/context/AttributeContext.hpp ;
ARTIST/include/graphic/opengl/validator/AttributeConcept.hpp ;
ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp.
GrepAI confirme Reader/contexte/concept ; aucune nouvelle preuve de bug exécutée.

Décisions restantes : alias/diagnostics à figer par A ; restitution QA à vérifier ;
ASan exclu tant qu'aucun protocole séparé n'est approuvé. Une migration large ou
un besoin de lifetime VBO impose révision du périmètre, pas extension silencieuse.

## Approbation

Aucun worker/classifier démarré, aucune tâche publiée, aucune source modifiée.
Demander `/ensemble-mission config` pour présenter la nouvelle politique native,
puis attendre `/ensemble-mission start` utilisateur. Ne jamais l'invoquer pour lui.
La pause opérateur nécessite en plus un GO de reprise distinct.
