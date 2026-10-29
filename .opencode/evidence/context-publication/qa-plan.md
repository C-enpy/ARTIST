# Publication du contexte existant — plan QA adopté

Mandat utilisateur : publier sur main tout le contexte local sauf `.tmp/`.
Exception de sécurité nécessaire : configuration GrepAI active avec identifiant
local conservée sur disque et retirée du suivi ; exemple sans secret publié.
Aucun moteur, shader, modèle approuvé, équipe ou grant à modifier.

Plan préparé en lecture seule par qa-planner natif fresh, session
`ses_e78877234ffeWWRN1BSXdj00tE`, finale effectivement reçue. Son essai de message
avait échoué sur une erreur de chaîne ; la finale native assure ici la remise.
Aucune édition ni exécution par le planner. Le lead est runner et intégrateur.

## Exigences et risques

Explicites : E1 publier les changements autorisés et préserver leur sens ; E2
exclure `.tmp` et identifiants ; E3 qualifier les contrôles du harnais sans faux
RED ou assertion affaiblie ; E4 distinguer archives, statique, runtime et GPU ; E5
fetch/push sans force sur main. Inférées : liste exacte/hash avant index et arrêt
sur changement concurrent, divergence ou échec non trié.

- R1 clé publiée, impact5 × probabilité3 =15 : ajout global/config encore suivie.
  Détection : scan limité puis revue des blobs indexés ; mitigation : retrait de
  l'index seulement, hash local avant/après. Gap : pas d'audit exhaustif ni purge
  de l'historique déjà partagé.
- R2 faux vert du harnais,4×3=12 : oracle et profils incohérents ou statique
  présenté comme enforcement. Détection : suites T04–T13 et modes syntax/schema/
  offline, négatifs causaux. Mitigation : gardes conservées, aucun nouveau droit.
- R3 perte/omission/divergence,4×3=12 : index incomplet ou push destructeur.
  Détection : manifeste des fichiers, empreintes, diff indexé et refs ; mitigation :
  exclusion explicite des temporaires, préservation live config, push normal FF.

## Preuves et oracles

P1 inventaire/diff/hash→E1 ; P2 exclusion index/secret + hashes locaux→E2 ;
P3 dix suites Node existantes + trois modes intégrés→E3 ; P4 profils/oracles et
mutations de permissions→E3/E4 ; P5 non-régression T06–T13→E1/E3/E4 ; P6 JSON,
sources/liens et provenance des exports→E4 ; P7 refs et SHA distant→E5. Sens
inverse identique, aucun test moteur orphelin ajouté pour ce lot documentaire.

Nominal : les trois profils ont la même politique communication v2 déjà choisie
dans AGENTS. Négatifs : anciennes règles sans transport, élargissement shell/edit/
subagent/externe, ordre/identité/path/manifest altéré, faux grant et canari absent
doivent être rejetés pour le motif attendu. Un PASS statique reste statique.

## Divergence reproduite et disposition du lead

Avant toute nouvelle correction :594 tests exécutés,589 PASS,5 FAIL,0 skip ;
syntax/schema intégrés PASS, offline FAIL pour les deux profils autres que le
planner. L'implémentation et les oracles reconnaissent v2 seulement pour le
planner alors que AGENTS et les trois profils imposent déjà le même contrat.
Logs `out/logs/context-publication/agentic-contract-tests.log` et `agentic-offline.json`.

Disposition : correction séparée bornée du validateur et de ses consommateurs de
test pour reconnaître le contrat v2 identique des trois profils. **Aucune permission
de profil ne sera modifiée**. Les directives de projet imposent déjà cette égalité,
il ne s'agit pas d'un nouveau choix de politique ou modèle. Un négatif/nominal
v2 ciblé sera fixé dans les tests, exécuté RED sur le validateur actuel, puis
GREEN minimal et non-régression. Les anciens cas mutés restent refusés ; aucun
template chargé depuis le candidat ni évaluation native maison. Revue indépendante
fresh ciblée avant publication pour vérifier l'absence d'affaiblissement.

Ownership local indivisible : lead, `.opencode/scripts/validate-agentic.mjs`,
tests/oracles/canaris déclaratifs T04/T05 et note de politique contradictoire.
Méthode PM/LLM locale : complexité brute58, retenue70 ; risque60. Pas de worktree
auteur ou équipe artificielle. Serena confirme ARTIST mais ne couvre que C++ ;
ces fichiers JS cachés sont suivis par grep exact/lectures et consommateurs CLI.

## Séquence et sortie

Persister/commiter ce plan avant les edits auteurs ; RED ciblé identifié ; GREEN
minimal ; suites complètes et négatifs ; revue indépendante ; JSON/liens/secrets ;
index exact hors `.tmp`/live config ; fetch, commit/push FF, SHA distant et hashes
locaux finaux. Aucun résultat manquant transformé en PASS.

Budget relatif : setup25 unités, puis75% planifié/25% buffer sur le reste ; pas de
durée inventée. Pas de test moteur, nouveau harnais, nouvelle campagne GPU ou
preuve de hot reload/enforcement revendiquée. Toute extension indispensable au
contrat ou conflit non résolu revient au lead puis à l'utilisateur si nécessaire.
