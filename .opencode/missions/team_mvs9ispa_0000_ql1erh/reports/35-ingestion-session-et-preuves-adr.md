# Ingestion de la session Linux et de ses sous-agents

## Objectif et périmètre

Conserver le contexte réutilisable de la mission Linux QA, de ses revues et de sa
publication, puis intégrer ses enseignements aux notes existantes et aux preuves
des ADR. La réalisation moteur et la fusion sont terminées ; cette tranche est
documentaire et ne relance aucun test moteur ou agent d'implémentation.

Le checkout principal est `main`, révision `f80c7cb`. Seuls des fichiers locaux
`.tmp/` préexistent hors Git. Ils restent intacts. `.wiki.json` et AGENTS désignent
`docs/`, `research/` et la collection `artist-wiki` ; la configuration wiki globale
désigne un autre projet et ne sera ni suivie pour l'écriture ni modifiée.

## Plan local et dépendances

Travail documentaire indivisible, méthode PM/LLM locale : complexité documentaire
brute/retenue 42/100, risque 35/100. Pour les scripts ponctuels d'export/contrôle
non livrés sous `.tmp/`, estimation brute 42, retenue **70** par application du
plancher développement, risque 35. Pas de faux RED comportemental pour une
ingestion : contrôles de syntaxe et cas discriminants de filtrage/identité adaptés.
Le contrôle porte sur sources, hashes, métadonnées, liens et récupération QMD.

1. En parallèle logique : retrouver les pages existantes avec QMD et inventorier
   les sources publiques natives. Aucun worker supplémentaire n'apporte ici de
   contrôle spécialisé distinct ; les remises indépendantes existantes sont les
   sources à attribuer, pas des tâches à relancer.
2. Exporter l'échange public principal et le texte assistant des quatorze enfants
   directs découverts, avec pagination jusqu'à épuisement et borne exclusive de
   la demande d'ingestion. Exclure briefs internes, instructions synthétiques,
   reasoning, outils, pièces jointes et secrets. Les exports ne sont pas un backup
   complet ni une preuve d'exécution. Pas de revendication d'inventaire récursif.
3. Vérifier les empreintes contre le manifeste existant ; ne pas réingérer les
   onze exports inchangés de la session float préparé. Distiller les nouveaux
   apports dans une note de session et les notes métier pertinentes.
4. Actualiser les preuves ADR sans changer leurs statuts ni généraliser la
   qualification Linux logicielle à du matériel, Release, fuites ou performance.
5. Mettre à jour index, journal et manifeste ; contrôler les liens, sources et
   métadonnées, puis update/embed ciblés `artist-wiki` et récupération MCP.

L'export et la lecture documentaire sont indépendants ; l'intégration exige
leurs deux résultats. Manifeste et index QMD ont chacun un seul écrivain. La
preuve de sortie sera le diff documentaire contrôlé et sa récupération, pas les
anciens PASS moteur présentés comme nouvellement exécutés.

## Limites conservées

Six campagnes initiales à 589 tests distinguées des quatre campagnes renforçant
l'identité shader ; contrôleur Python 10/10 distinct du harnais agentic 634/634.
Linux graphique = Mesa llvmpipe ; ASan sans détection de fuites. Le RED initial
classification incomplet, l'incident d'incrémentalité Windows et le fallback 3.3
ambigu restent signalés. ADR-0001/2/4/6/7 Proposed, 0003/5 Accepted/partial,
0008 vérifié dans sa baseline Windows Debug seulement.
