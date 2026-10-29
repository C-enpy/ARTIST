# Huit ADR audités — candidat documentaire consolidé

## Résultat à contre-vérifier

Les deux enquêtes ont remis leurs conclusions. Elles sont intégrées dans
`docs/references/adr-audit-2026-10-29.md` : 106 dispositions couvrant décisions,
migrations et critères des huit ADR, avec applicabilité et preuves/limites.
Ce nombre est celui des lignes de traçabilité, pas un nombre de tests réussis
ni une preuve automatique d'exhaustivité ; la contrelecture vérifiera le fond.

Deux champs implementation évoluent : **0001 et 0007, not-started → partial**.
Les statuts de décision restent motivés séparément. ADR-0002 est corrigé : la
séquence N=2 conserve des IPass polymorphes, les bindings float ne sont plus
totalement absents. ADR-0005 reste partial pour le chemin coverage ON ou son
rejet ciblé non prouvé, et non pour une matrice de plateformes idéale.

ADR-0003 conserve des chemins d'échec de préparation non tous exercés ; ADR-0004
n'a pas de mesure d'allocation acquise ; ADR-0006 n'a pas de campagne comparative.
ADR-0008 reste verified pour la baseline Windows Debug adoptée. Aucune API future,
RAII exhaustive, Release ou matériel Linux n'est ajouté comme critère artificiel.

## Remises récupérées, pas auteurs relancés

Le collecteur signalait deux membres inactifs sans réponse finale. L'inspection
native a trouvé dans chacun une session succeeded et une unique finale publique
finish=stop. Deux pages assistant par session, jusqu'à épuisement ; textes et
hashes sauvegardés dans `.opencode/evidence/adr-audit-2026-10-29/public-deliveries.json`.

- Architecture : `ses_e78453ac8ffeQ4BwxZsy3q9XaJ`, finale
  `msg_187bba8b5001dGHt2MmgSLHp9u`, environ 20,5 k caractères.
- Build : `ses_e7844979effe7hD9b4cvcERSNy`, finale
  `msg_187bbd6a5001Tfn9qcRk7LUX35`, environ 7 k caractères.

Aucune relance, réparation de runtime, modification de permissions ou de modèle.
Les finales excluent reasoning et outils ; leurs conclusions restent des sources
à recouper. Le lead a décomposé les regroupements en obligations et borné les
conditions futures. Aucun auteur n'a écrit dans le checkout.

## Contrôles exécutés par le lead

`C:/Python313/python.exe -B .tmp/check-adr-audit.py` : **451 assertions
documentaires**, 220 liens de fichiers, huit états/dates/origines, hashes des
finales, 106 identifiants de clauses uniques, YAML/JSON/frontmatter/tags/sources,
journal append-only et absence de changement exécutable aux chemins déclarés.
`git diff --check` propre. Ce contrôle ne remplace pas la revue du raisonnement.
Preuve : `out/logs/adr-audit/document-checks.json`.

Comparaisons Git : runtime inchangé depuis78e7b74 aux chemins ARTIST, données,
CMake/presets/Conan/workflow ; scripts contrôlés inchangés depuisf58a49f.
Serena confirme ClosedExecution et ses appels ; ses références étaient
incomplètes, complétées par grep exact dans les tests. Les bases de compilation
Windows existent, 63/64 entrées normal/ASan ; cela ne prouve pas une couverture
complète de l'index C++ ou une nouvelle exécution.

GrepAI a renvoyé des rapports pour la recherche benchmark ; son horodatage
d'index était ancien, donc pas preuve d'absence actuelle. Contrôle exact complété
sur les marqueurs usuels dans C++/CMake ; l'unique steady_clock retrouvé nomme
une fixture Reader. Aucun benchmark n'est déduit des comptes ou durées CTest.

## Limite de contrôle locale non masquée

Le contrôle initial d'empreinte de la configuration GrepAI privée a échoué par
rapport à la photographie prise lors de la publication précédente : l'horodatage
local d'index est désormais plus récent après usage du service. La configuration
ancienne complète n'est pas archivée ; **identité octet historique INCONCLUSIVE**,
sans prétendre que seul cet horodatage a changé. Pas de restauration ni réparation
de configuration hors mandat. Elle reste ignorée et non suivie, identifiant local
absent des candidats publics ; les deux anciens temporaires protégés sont identiques.
Le verdict des contrôles documentaires n'est pas un PASS global d'intégrité locale.

## Suite autorisée

Figer/intégrer le candidat documentaire et ses sources, puis contrelecture
correctness-reviewer fresh, sans auteurs/briefs privés et sans exécution de tests.
Les corrections documentaires éventuelles reviennent au lead. Les anciennes
tâches deferred restent inactives ; aucun test moteur, configure, CI, image ou
benchmark n'a été relancé pour cet audit.
