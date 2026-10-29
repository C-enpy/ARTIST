# Plan initial archivé — mission float OpenGL préparé

État final du 2026-10-27 : politique native v1 approuvée, source `3b1b102`,
581 normaux/581 ASan, ré-audit borné accepté et six livrables du board complétés.
[Bilan final et limites](reports/24-bilan-final-float-prepare.md).

Le contenu de préparation ci-dessous est conservé comme historique du graphe,
des scores et des barrières. Les mentions « non approuvée » ou « non publiée »
décrivent cet instant initial, pas une demande de redémarrage de la tranche terminée.

## Photographie de préparation conservée

Date : 2026-10-27. Lire contract.md et qa-plan.md dans ce répertoire.
La politique est encore un brouillon ; aucun auteur ni classifier ne démarre.
Import natif contrôlé : qa-contract → task_mvqmfwpt_0001_vim66t, seule tâche
pending, profil fresh/worktree false/final_response, scores66/76. Aucun worker.
Les cinq autres étapes restent documentaires, non publiées ; leurs noms locaux
ne sont pas des IDs Ensemble. Voir reports/05-preparation-importee-controle-superviseur.md.

## Décision de publication échelonnée du coordinateur

Importer au départ uniquement la première tâche qa-contract, avec complexity66,
risk76, agent qa-planner, context fresh, worktree false et final_response.
Les cinq autres fiches restent dans le présent plan, non publiées. Après start
utilisateur et intégration/validation de chaque entrée, le lead les ajoute par
team_tasks_add avec les champs structurels et les vrais IDs déjà retournés.
L'import initial avant start reste distinct d'une admission après start.
La publication tardive, pas une simple dépendance de tâches déjà importées,
protège ici les barrières d'intégration. Aucune deuxième importation globale.

Les paramètres natifs et conditions exactes sont conservés dans
[publication-plan.json](publication-plan.json), un plan local, pas un document
à soumettre directement à l'importeur. Le graphe complet reste décrit ci-dessous.

## Sources et évaluation réutilisées

Architecte : ses_e838c7f7cffeweoAHqj24DDdC4 ; PM :
ses_e838c3676ffe32Z9Mt5KwsJHk0 ; proposition QA conditionnelle :
ses_e838b2cb0ffeKTjoF0uoSPLjT8. Remises reçues dans la conversation.
HEAD/diff et statuts ADR : contract.md. Les fiches affinent le plan PM déjà reçu,
sans nouveau planner, enquête redondante ou création artificielle d'équipe.

Méthode PM/LLM, scores bruts/retenus séparés du risque 0–100. Plancher 70 pour
tout développement, code de test et instrumentation exécutable compris. Ces
estimations sont conservées selon ARTIST ; aucune activation de classifier/JEV,
aucune modification des métriques de modèles ni override de spawn. Le routage
exact relève du catalogue natif approuvé, pas de ces nombres présentés en prose.

## Ordre et ownership

### Préparer les entrées et qualifier la QA — coordinateur

Réutiliser les enquêtes architecte/PM/consommateurs déjà faites. Réconcilier le
plan QA, vérifier admission réelle (profil, contexte, lecture, restitution),
fixer observabilité et oracle pixel, persister et committer le paquet avant
worktrees auteurs. Aucun droit élargi pour débloquer un profil restrictif.
Pas de nouveau build-engineer/gpu-engineer pour refaire une enquête acquise.

Ressource exclusive : index Git pour commits sélectifs des seuls fichiers possédés.
Ne pas embarquer les diffs FOREIGN. Absence de voie native conforme : BLOCKED,
solution bornée présentée, pas de chantier d'outillage.

### Développer l'accès préparé et sa sûreté — cpp-engineer

Objectif : API float seule, témoins, invalidation, publication et maintien du
miroir CPU ; tranche RED/GREEN/REFACTOR complète par un même auteur.
Scores PM : complexité brute/retenue 84/84 ; risque 90. Incertitudes : détail
local des témoins et observabilité de la résolution, pas architecture ouverte.
Contexte fresh, worktree isolé, HEAD et commit de préparation explicitement fournis.

Ownership : PassContext commun, contexts OpenGL Pass/Uniform, composants
pass Loader/Freer/UniformReader et uniform Validation/Setter strictement nécessaires,
nouvelle API OpenGL préparée, tests CPU/lifetime et composants dédiés à cette API.
Pass.hpp/Uniform.hpp seulement si nécessaire à la coexistence adoptée.
Pas Attribute, Pipeline, Reader fichier, scripts/toolchain ou profils QA.
Nouveaux chemins précis confirmés dans le plan local de cet auteur avant édition.

Invariants/binding times : contract.md. Tests disponibles : UniformValue,
UniformContract, UniformOwnership, UniformReader, UniformCardinality/Rejection,
Pass lifetime et leurs composants GL. Ajouter tests positifs/négatifs, stale avant
déréférence, même ID simulé et échecs transactionnels. RED comportemental causal
observé avec préconditions valides, GREEN minimal, puis régression ciblée.

Exclusivité : un écrivain par fichier, un build par sortie, bail GPU entre tous
processus/worktrees. Sérialiser l'exécution ; ne pas inventer une preuve à partir
d'un refus d'environnement. Localisation symbolique limitée : refs Serena vides
malgré usages textuels et diagnostics de configuration, donc grep exact complément.

### Intégrer le mécanisme — coordinateur

Auteur arrêté et livrable intégré par voie native, diff contrôlé, candidat et
preuves réellement accessibles. Publier le consommateur seulement ensuite avec
le vrai ID du producteur et ce HEAD intégré. Pas de tâche factice pour cette barrière.

### Migrer le consommateur et prouver le résultat — gpu-engineer

Objectif : target préparé dans ReflectedProgramGuardCrossTests.cpp, decoy par nom,
readbacks et scène pixel utilisant le programme f2Scalar existant ; exemple cohérent.
Scores PM : complexité 74/74 ; risque 77. Incertitudes : oracle pixel et comptage
doivent être fermés avant édition, pas ajustés pour obtenir le vert.
Contexte fresh, worktree isolé depuis mécanisme intégré et plan QA commité.

Ownership : ce consommateur, test image dédié/fixtures possédées et exemple ;
pas les headers moteur de l'auteur précédent. Bug production : retour à son owner,
réintégration et nouvelles preuves, pas correction concurrente par le consommateur.
Critères : séquences distinctes de floats, miroir CPU, target/decoy inchangés après
rejet, comptages séparant préparation/updates et CPU/native, contrôle image sensible
à une modification de valeur. RED puis GREEN sans affaiblir les assertions.
Ressources : build propre à la branche et bail GL sérialisé.

### Intégrer tous les producteurs — coordinateur

Même barrière native, diff relu et preuves disponibles. Fin d'auteur seule
insuffisante. Dépendances des consommateurs ultérieurs non publiées avant ce point.

### Relire la sûreté et compléter les gaps — deux lectures indépendantes

correctness-reviewer fresh : contrat adopté et diff intégré neutres, pas conclusions
d'autres reviewers. Complexité 76/76, risque 85. Lire ownership, générations,
publication, contexte et erreurs ; revue statique distincte des tests exécutés.

test-engineer fresh : après intégration seulement, oracles avant propositions de
tests, KEEP/MODIFY/REJECT/DEFER, aucune production modifiée. Complexité 67/70,
risque 68. Proposition texte/patch au lead, jamais runner autonome ni substitut
au RED auteur. Profil/restitution restrictifs à qualifier avant admission.

Ces deux lectures peuvent être parallèles sur le même snapshot, sans écriture
ni ressource GPU. Pas de board d'implémentation exposé au futur audit aveugle.

### Exécuter et auditer le candidat final — lead puis test-reviewer fresh

Lead : relire/persister les propositions retenues et exécuter les tests sous mandat
distinct ; auteur de proposition et runner attribués. Complexité 73/73, risque 74.
Défaut production renvoyé au propriétaire. Au plus deux réparations healer bornées ;
ne pas masquer un bug réel ni poursuivre une répétition sans hypothèse nouvelle.

test-reviewer : admission fresh hors board d'implémentation, contexte candidat /
contrat / preuves neutres, sans justifications d'auteurs ni conclusions précédentes.
Complexité 72/72, risque 81. Audit de discriminance et des limites, sans exécution.
Finding bloquant : propriétaire, intégration, runner puis ré-audit. Suggestion
non bloquante ou gap hors mission ne déclenche pas d'extension automatique.

### Documenter et clore — coordinateur

Code, tests, exemple et limites ; adoption limitée et preuves ADR-0004, registre,
état courant, pointeur prochaine mission. Pas de statut verified global ni gain
de performance. Vérifier à la fois livraison intégrée, preuves et état natif des
tâches ; pas de completion fictive, purge, push ou nettoyage de diffs étrangers.

## Graphe et parallélisme

Préparation/QA qualifiée et commit → mécanisme → intégration → consommateur/pixels
→ intégration → revue de sûreté ET analyse des gaps en parallèle → runner lead
→ audit fresh et éventuelle boucle de corrections → documentation/clôture.

La séquence des auteurs est une dépendance réelle de l'API et de son snapshot.
Les barrières d'intégration et opérations du lead ne sont pas des agents ni des
tâches artificielles. Aucun auteur supplémentaire pour maximiser le parallélisme.

## Admission à vérifier, sans réparation incluse

Le paquet corrigé annoncé par la session plugin est désormais corroboré sur un
point dans ce catalogue : team_tasks_add expose worktree/report_mode en plus des
scores/agent/context. Le dernier candidat PM encode aussi ces champs sur ses six
fiches. Le transport n'est plus le blocage de préparation retenu. La qualification
QA effective reste à faire après start, en première tâche et avant auteurs.
L'arbitrage de publication ci-dessus résout la question restante du candidat,
sous réserve de sa reprise/validation native ; le lead ne fabrique pas ready.

Mise à jour après candidat PM natif bloqué : voir
[le constat d'admission](reports/02-blocage-admission-preparation-pm.md).
La qualification QA est un livrable avant auteurs, pas une preuve déjà acquise.
La présence de worktree/final_response au spawn ne résout pas à elle seule
leur raccord à l'import ni la course éventuelle avec l'auto-dispatch.

Le catalogue précédemment découvert expose agent/context/scores dans l'ajout,
worktree/report_mode au spawn, mais pas require_context dans team_tasks_add.
Vérifier la voie native réellement exposée avant publication ; ne pas inventer
de champ, supposer une application effective ou tenter en boucle un refus connu.
Rester en préparation si le contrôle requis n'est pas réalisable. Le start
natif ne dispense pas des barrières ARTIST ni des profils restrictifs.
