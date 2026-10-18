# Clarification permanente du relais QA — périmètre documentaire

Autorisation utilisateur : adaptation permanente des profils, 2026-10-18.
Contrat local appliqué par le lead (petit travail indivisible) : PM/LLM,
complexité brute/retenue 32/32, risque 42. Aucun développement exécutable :
consignes Markdown seulement, sans modifier permissions, modèles, variantes,
politique approuvée, routage, plugin ou paramètres globaux.

Objectif : ne plus confondre board in_progress et worker natif actif ; relever
les réponses finales QA autorisées, avec attribution, avant clôture lead.
État initial : HEAD 4a9b722, modifications documentaires de mission préexistantes.
Ownership : corps Markdown des profils orchestrator/qa-planner/test-engineer/
test-reviewer uniquement. Les frontmatters et leurs 27 règles QA sont immuables.
Ressources exclusives : écriture de ces quatre profils par le lead, aucun build.

Source diagnostique : qa-delivery-diagnostic.md adjacent, messages natifs et
littéraux du plugin chargé. Pas de preuve d'un bug d'affichage. La contradiction
de restitution est distincte du timeout Cursor dont la cause interne reste inconnue.

Vérification documentaire, sans RED comportemental artificiel :
- comparer les quatre frontmatters à HEAD, octet pour octet ;
- compter 27 règles dans chacun des trois QA ;
- contrôler le diff et ses espaces ;
- vérifier que la politique approuvée et le plugin n'ont pas été modifiés.

Nominal : un rapport final existe ; le lead relève ses parties textuelles,
persiste et relit, puis clôture le board sans faire relancer le worker.
Négatif : absence de voie de lecture ou rapport incomplet => BLOCKED, pas de
nouvelle permission ni de PASS implicite. Les décisions humaines restent ouvertes.
Cette clarification n'est ni une réparation du plugin ni une preuve d'exécution
autonome du nouveau protocole. Une telle correction exige un lot distinct avec
plan QA préalable et tests exécutables. Les sessions déjà créées ne rechargent
pas forcément ces consignes ; ne pas prétendre avoir changé leur runtime.

## Contrôles réellement exécutés

Comparaison des quatre frontmatters à `git show HEAD:<path>` : 4/4 identiques,
empreintes SHA256 conservées ; trois QA à 27 règles exactes chacun. Sections de
relève trouvées dans les quatre profils. `git diff --check -- <quatre profils>`
sans erreur. Diff : 60 lignes ajoutées, aucune permission modifiée. Ces contrôles
prouvent la cohérence documentaire, pas une clôture native autonome du plugin.
