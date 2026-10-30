# Proposition de tests consommateur — contexte CTest historique

## Candidat GREEN de composition — 2026-10-30

Le lead a observé les deux RED (20 méthodes/33 failures, puis trois méthodes/13
failures) et ratifié le mapping d'une seule assertion dans `f4021cc`. Le candidat
minimal est décrit dans `CONTEXT_COLLECTOR_GREEN_READY.md` : préflight pur des six
variables, vrai parser par cas/invocation, populations/causes et relecture readonly.
Aucun test exécuté par l'auteur ; GREEN/non-régression à observer par le lead.
Le plan et les remises historiques ci-dessous restent conservés et attribués.

## Actualisation de réception — 2026-10-30

Les conditions de réception du cœur et du parser sont maintenant satisfaites :
base `3bd72fc`, fondations reçues dans
`107a2d3bbd88ba95a2eb70e904085911b71662bd`, rapport73 intégré. Les adaptateurs
de production ne sont pas requis pour le RED local : composition par service
factice explicite, sans copie de branche et sans lancement d'outil natif.

Ownership de cette tranche : nouveau `test_context_collector_contract.py`, ce
plan et sa remise `CONTEXT_COLLECTOR_RED_READY.md`. Après observation RED par le
lead seulement : `_junit`/agrégation de `evidence.py`, relecture de `reporting.py`
si nécessaire et retrait conditionnel de la garde CLI. Aucun changement runner,
schéma public, parser/producteur, adaptateur, fixture C++ ou assertion existante.

Séquence bornée : tests C1–C5/C7 et liaison d'invocation C6 → RED lead → GREEN
minimal de composition → non-régression lead → revue indépendante. Le transport
réel C6 reste une barrière d'intégration adaptateurs distincte ; les sentinelles
list/plan et le filtrage des six noms réservés existants ne sont pas remplacés.

La suite nouvelle comporte 20 méthodes statiquement recensées, pas 20 résultats
observés. Aucun test exécuté par l'auteur. Voir la remise autonome pour commandes,
RED attendu, mappings QA et limites. Le texte ci-dessous conserve le plan et les
préconditions historiques, désormais remplacées par cette actualisation.

## Plan historique conservé

Statut : préparation documentaire uniquement, pas RED_READY exécutable.
Base cœur locale : `b48947abecf7f65f48a92df8a9691dccdadf4ac9` (R2 remis,
GREEN à observer par le lead). R1 reste bloquant et sans implémentation anticipée.

## Entrées reçues et frontière

- Producteur/parser intégrés au lead :
  `91c5ac23bf17d4d88dd36699cb7d15a6d6835872`, six fichiers reçus identiques au
  candidat revu `057b8417933cdcbe10b9c16e3dffd4c56acd569a`.
- Réception et limite GoogleTest :
  `317f36be2a1fde27f3a715aa4f1f682cdc7fb260`, rapport60 et README du producteur.
- Contrat et QA §10/A3–A8 : `50af7923ff9094ac8e7a175d3256126204b4da41`.
  Contrat et documents de réception lus dans les commits, sans transfert Git.
- Les 28 tests parser PASS sont un résultat communiqué par le lead après merge,
  pas un run de ce worktree. C++ NON_COMPILE, GL NON_EXECUTE.

L'interface ratifiée est `parse_context_observation(text, *, expected) -> dict`.
`text` est le `system-out` d'UN cas JUnit, jamais stdout global ou concaténation
de cas. `expected` contient exactement `run_id`, `plan_sha256`, `request_id`,
`ctest_name`, `profile`, `expected_gl_vendor`, `expected_gl_renderer`.
Absence, doublon, troncature, JSON/types/identités étrangers : `ValueError` du
parser puis panne de protocole code4 au collecteur, même si l'enfant retourne0.
Le statut de précondition et les assertions JUnit restent au collecteur.

Le présent fichier ne livre aucun collecteur, import du parser, nouveau champ de
schéma ou assouplissement de garde. `unsupported` CTest hardware reste bloqué
avant adapter/sonde/enfant jusqu'au raccordement effectif accepté.

## Conditions de départ de la tranche future

1. Recevoir le contrat complet R1, son correctif capture/ownership et la réception
   du cœur selon le lead. Le plan ne vaut pas autorisation de capture anticipée.
2. Au point de repos, le lead compose un arbre contenant le cœur, les adaptateurs
   et le parser réellement intégrés. Ne pas copier/cherry-pick une branche ni
   traiter un ImportError comme le RED attendu. Vérifier les identités reçues.
3. Écrire les tests ci-dessous dans le fichier proposé
   `scripts/tests/verification_core/test_context_collector_contract.py`, en
   réutilisant les fixtures existantes ; adaptations strictement nécessaires
   à coordonner, sans altérer les assertions acquises ou dupliquer le parser.
4. Le lead observe le RED causal, puis autorise le GREEN de composition. La garde
   ne se retire pas sur la seule présence/importabilité du parser.

## Cas proposés et oracles discriminants

### C1 — parcours nominal composé, pas simple rejeu d'un double

Un faux enfant possédé produit découverte + JUnit complets pour les noms réels
sélectionnés par le catalogue. Les objets contexte synthétiques stricts sont
dans chaque cas OpenGL hardware, exactement une ligne `[ARTIST_CONTEXT_V1] `.
Le vrai parser livré est utilisé : observations exactes, qualification conforme,
précondition OK et population concordante donnent PASS/code0. Vérifier les octets
JUnit/logs conservés, les reçus et le report readonly. Aucun chargement GL.
Ce test atteint la composition réelle ; il ne prouve pas la garde native.

### C2 — attribution par cas et par invocation

À partir du nominal, une mutation à la fois : ligne absente, ligne double,
JSON tronqué ; permutation d'observations entre deux cas ; run/plan/request/nom
étranger ; cible vendor/renderer différente ; ligne uniquement dans stdout global
ou dans le system-out d'un autre cas. Tous refusent le favorable, code4 attendu.
Le collecteur ne remplace jamais une identité absente par le nom attendu.
Une seconde invocation/candidat ne réutilise ni observation ni état de la première.
Ne pas reproduire les 28 tests unitaires de syntaxe déjà détenus par le parser.

### C3 — population et niveau de qualification

L'observation est exigée pour CHAQUE cas OpenGL hardware sélectionné. Absence de
cas, duplication JUnit, nom étranger et sélection/découverte incohérente restent
des défauts de population. Des cas CPU n'ont pas à émettre une observation GPU.
Ne pas transférer une preuve de contexte d'un cas à un autre, ni conclure hardware
depuis un libellé renderer ou un préflight. Le chemin logiciel conserve ses gardes
propres : aucune activation matérielle implicite ni repli logiciel silencieux.

### C4 — précondition, oracle et priorité stable

Précondition structurée cohérente, attribuée et accompagnée du skip attendu →3,
pas FAIL d'oracle. Assertion réelle dans un autre cas + précondition →5, avec
causes et population partielle conservées. Protocole invalide + assertion →4 ;
interruption →130. Permuter l'ordre des cas garde codes, causes et population
équivalents. Skip ordinaire sans preuve structurée et code enfant incompatible
ne deviennent jamais un favorable. Ne pas classifier depuis le texte de reason
ni depuis le seul code de sortie. Établir les résultats attendus sur QA A6 avant
d'adapter la réconciliation historique qui rejette aujourd'hui les skips.

### C5 — exécution interrompue et limite GoogleTest

`catch_exceptions=0` peut interrompre le corps sans garantir TearDown ou JUnit
complet. Fixtures synthétiques : JUnit absent/tronqué, observation absente ou
préfixe INVALID_REQUEST seul. Aucune de ces situations n'est une précondition
structurée valide : absence de preuve interdit le code3 fondé sur cette preuve
et tout favorable ; panne/protocole ou interruption selon les faits conservés.
Le collecteur ne fabrique ni observation, ni fin de test, ni preuve de retraite
de fenêtre/contexte depuis le seul arrêt du processus.

### C6 — transport, liaison et pureté au point de composition

Reprendre le contrat acquis des six variables retirées avant env_delta : aucune
activation héritée ; plan/reçus donnent les seules valeurs explicites ; request_id
distinct par invocation, cible exacte Unicode/espaces sans normalisation. Vérifier
les descriptions adapter puis l'environnement capturé par le fake runner.
La cible exécutée provient des chemins validés du BuildReceipt ; argv0 PowerShell
ne prouve pas l'identité du workload. Aucun nouveau champ core n'est proposé.
List/plan/descriptions ne chargent ni parser workload, ni adapter à effets, ni GL ;
runner manquant ne déclenche aucun fallback. Garder les sentinelles existantes.

### C7 — conservation et relecture sans cache

Conserver le JUnit original, la sortie brute et leurs identités, y compris lors
d'un rejet. Sans processus, sonde de révision ou écriture, le report relit les
preuves ; mutation persistante JUnit/identité n'est pas acceptée via cache de
qualification. Réutiliser les contrôles d'intégrité receipt/index existants,
sans introduire un format contexte concurrent ou recopier le schéma workload.

## Ownership, erreurs, confinement et preuves

Le bundle/sink possède les fichiers bruts ; le parseur consomme une chaîne locale
du cas et retourne des données validées, sans emprunt à un arbre XML mutable ni
cache inter-cas. Le collecteur ne possède pas de ressource GPU ; les durées de vie
contexte/fenêtre appartiennent au producteur. Le runner reste responsable de ses
handles, lecteurs et buffers selon R1, indépendamment du verdict du collecteur.
Un XML/JSON incomplet ne doit pas publier une qualification partielle favorable.

Les futurs tests n'utilisent que données temporaires possédées et doubles/éventuels
programmes Python factices déjà autorisés, sans enfant natif, CTest réel, moteur,
build, GL, benchmark, installation, cloud ou expérience parent/descendant. Aucun
nouveau runner ou harness général. Un fichier proposé n'est pas un test découvert.

Commandes futures pour le lead, uniquement après composition et écriture des tests :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_collector_contract.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
```

Aucune de ces commandes n'a été exécutée par l'auteur. Les 134 tests/fixtures et
toute production locale sont inchangés. R2 GREEN reste à observer ; R1 et la
composition sont NON_EXECUTE. Même un GREEN synthétique futur ne qualifiera ni
Windows/Linux natif, ni rendu, ni accélération matérielle ou performance.
