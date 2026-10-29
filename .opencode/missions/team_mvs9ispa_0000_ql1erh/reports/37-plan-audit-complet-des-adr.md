# Audit complet des huit ADR — plan et critères de sortie

## Objectif

À la demande de l'utilisateur, réévaluer chaque ADR sur les faits, et non conserver
ses champs par simple prudence d'ingestion. Distinguer adoption de la décision,
réalisation du contrat, preuves exécutées et conditions futures non déclenchées.
Cette continuation est documentaire : aucun changement moteur, permission,
politique modèle ou nouvelle mission native. Équipe existante approuvée conservée.

Base : `main` à `3d3e508f573f4738cb769d1afe5d6c2447b6f379`, diff suivi vide ;
les fichiers `.tmp/` restent hors publication. Les preuves récentes et les
archives sont disponibles dans ce checkout ; leur présence et portée seront
contrôlées, pas présumées à partir du résumé de conversation.

## Plan PM accepté

Préparation en lecture seule par project-manager natif fresh,
`ses_e784b31b0ffeFI13E5FzW1hNzB`, finale reçue. Méthode PM/LLM, scores
bruts = retenus, pas de classification JEV ou changement de modèle :

- Enquête architecture, ADR-0001/2/3/4/6/7 : **C76/R46**, engine-architect,
  lecture seule, aucune écriture/shell/build/test. Restitution native finale.
- Enquête qualification, ADR-0005/8 : **C63/R44**, build-engineer,
  même restriction, indépendante de la précédente.
- Consolidation et éditions documentaires : **C64/R48**, lead seul écrivain.
- Contrelecture du candidat intégré : **C61/R39**, correctness-reviewer fresh,
  lecture seule ; publication seulement après disponibilité du vrai candidat.

Aucun développement ou nouveau script livré. Ces scores de recherche/documentation
ne sont pas abaissés pour contourner le plancher développement. Pas de comportement
changé, suite étendue ou risque PM>=50 : aucun déclenchement d'une nouvelle boucle
QA d'auteurs ; la contrelecture porte sur la qualité distincte des conclusions.

Contrôle documentaire ponctuel non livré, exécuté seulement par le lead sous
`.tmp/` : réutilisation des règles de métadonnées/liens/provenance de l'ingestion,
plus vérification des huit états et identités des remises. PM/LLM local : brut40,
retenu70 pour ce script exécutable, risque35 ; pas de nouvelle infrastructure de
tests, pas de faux RED comportemental pour une modification documentaire.

## Graphe et ressources

Les deux enquêtes partent en parallèle : fichiers partagés en lecture seulement,
sans compilation, contexte GPU, indexation ou fixtures à réserver. Le lead persiste
leurs conclusions et recoupe les preuves avant les éditions. La contrelecture ne
part qu'après intégration et identité du diff documentaire. Les anciennes tâches
audit/documentation deferred/inactives ne sont ni relancées ni falsifiées.

Admissions réelles : enquête architecture `task_mvtjqeyb_0001_m5bj3c` affectée
automatiquement à pool-12/engine-architect avant le spawn explicite du lead ;
le refus de double claim a évité un doublon et n'a pas été répété. Enquête build
`task_mvtjqeyc_0002_yvrm3w` affectée par spawn à adr-build-audit/build-engineer,
worktree off et modèle/effort imposés par la politique v1, sans override. Les deux
restitutions sont attendues ; ce ne sont pas encore des résultats d'audit.

Les admissions portent explicitement agent, contexte fresh, worktree=false,
final_response et completion réservée au lead. Le catalogue actuel n'expose pas
le paramètre global require_context ; aucune activation de ce champ absent n'est
revendiquée. Chaque tâche utilise à la place son contrat de contexte effectif
exposé par l'API native. Aucune substitution de profil ou de modèle au spawn.

## Contenu obligatoire de l'audit

Pour chaque obligation décisionnelle, étape de migration et critère d'acceptation :
référence précise, applicabilité, état OBSERVED/PROVEN/NOT_IMPLEMENTED/NOT_RUN/
INCONCLUSIVE ou écart démontré ; fichier/symbole et preuve/candidat/date/OS/hash
disponibles ; source d'adoption, contradiction historique et dette restante.
Les recommandations de status et implementation sont distinctes et motivées.

Ne pas exiger une nouvelle API, Release, GPU Linux matériel ou RAII exhaustive
non promise pour empêcher artificiellement un verified. Une condition future
non déclenchée reste non applicable, pas une dette de réalisation actuelle.
Ne pas non plus assimiler un test existant, un compteur vert, une clôture de board
ou un document cohérent à la preuve complète de chaque obligation.

Les consignes initiales fixent les statuts de référence : Proposed pour 0001/2/4/6/7,
Accepted/partial pour 0003/5, verified Windows Debug pour 0008. Toute recommandation
divergente doit être explicitement sourcée ; aucune adoption globale déduite d'une
tranche adoptée ou de la seule demande d'audit. Les faits et champs implementation
obsolètes doivent en revanche être corrigés lorsque les preuves le justifient.

## Sortie et limites

Huit ADR et leurs critères tous disposés dans un registre détaillé lisible,
preuves valides réutilisées, claims non démontrés bornés, documents et liens
réconciliés, contrôle documentaire et récupération QMD ciblée. Pas de faux RED,
de nouvelle campagne moteur ou de mesure de performance fabriquée.
Les campagnes Linux llvmpipe et ASan detect_leaks=0 restent à leur portée ;
les trois populations 589 moteur, 10 contrôleur et 634 harnais restent distinctes.
