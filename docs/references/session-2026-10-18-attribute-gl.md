---
title: "Session Attribute-GL — résultats et enseignements"
category: reference
tags: [artist, rendering, testing, reference]
summary: "Session Attribute-GL et ses sous-agents : contrat des 12 formes, rectifications des oracles, retrait des sondes driver et limites de clôture."
created: 2026-10-18
updated: 2026-10-18
source_revision: f7bb171e1c8ca10929467a0fcd50469dbfa69837
sources:
  - "opencode:ses_eb1605bf2ffeAdDg1L6ZxSRQ90"
  - ".opencode/missions/team_mve3tz7l_000i_xtayy5/source-ledger.md"
  - "out/attribute-gl/q3-driver-probes/result.json"
provenance:
  extracted: 0.9
  inferred: 0.1
  ambiguous: 0.0
---

# Session Attribute-GL

## Périmètre de l'ingestion

La session racine `ses_eb1605bf2ffeAdDg1L6ZxSRQ90` et ses douze descendants
historiques accessibles couvrent **850 entrées natives : 416 lead + 434 enfants**.
La borne inclusive est `msg_150091c600010Tw3DVy0Rhrz5E`, demande d'ingestion.
Le nouveau PM d'ingestion et la rétrospective elle-même sont exclus. Les entrées
comprennent idle, synthetic et compaction : ce ne sont pas 850 messages humains.

Le registre assaini cité dans les sources inventorie les 13 sessions et leurs
messages de référence (E01–E12). Il distingue architectes, QA, PM, auteur,
build-engineer, generator, correctness-reviewer et revue fresh hors board.
Les 14 exports privés acquis, dont le PM exclu, ont été rapprochés de leurs
tailles/hashes/comptes ; ils ne sont ni publiés ni indexés dans le wiki.
Une compaction est présente : il s'agit de l'historique projeté accessible,
**pas d'un verbatim complet ni du journal interne du service**.

## Contrat et rectifications techniques

- **Douze formes CPU packed**, pas douze alias supplémentaires : float et
  vec2–4, int32 et ivec2–4, uint32 et uvec2–4. Le contrat prévoit Pointer pour
  les floats, IPointer pour les entiers, refus avant mutation GL et liaison du
  VBO propre malgré un leurre. Traits/layout/dispatch sont statiques ; réflexion
  après link et état VAO/VBO restent runtime. Source E01 : architecte
  `ses_eb151cd2cffef51CSQLI8Sy4g1`, `msg_14ebc79990017IOCMgL2ic2EMa`.
- **La fixture doit être légale avant de servir d'oracle.** Les vertex input
  arrays float/int/uint sont autorisés en GLSL 3.30 ; leur refus par ARTIST ne
  doit pas être confondu avec le rejet d'un shader illégal. Un tableau actif de
  cardinalité 1 requiert aussi l'observation du nom réfléchi. Source E02 :
  architecte `ses_eb0b8c427ffeoK2IS26ouPw1Wm`,
  `msg_14f5c4b63001gHmwzb5b8fQqeP` ; GLSL 3.30 §4.3.4.
- **Les petits entiers convertis en float32 ne démontrent pas l'exactitude
  entière générale.** Le premier plan proposait des captures représentatives ;
  l'amendement a demandé du transform feedback typé, des valeurs discriminantes
  autour de 2^24 et aux limites signées/non signées, sur les 12 formes. Sources
  E03 : même architecte, demande `msg_14f473e3f001cW7ZBmke21uSGa` et réponse
  `msg_14f5c4b63001gHmwzb5b8fQqeP`. Ce renforcement n'est pas une preuve portant
  sur toutes les valeurs possibles.

Ces contrats sont rapportés depuis la session. L'ingestion ne réexécute pas leurs
tests et ne revalide pas toute l'implémentation. Voir le
[[engineering-principles|cap technique]] et les [[adr/README|statuts ADR]].

## Rectification du cadrage : moteur versus driver

Une sonde attendait le refus d'une entrée vertex `struct` illégale ; le driver
NVIDIA l'acceptait. Elle ne sollicitait ni Reader, ni Setter, ni upload ARTIST.
Le lead a reconnu avoir fait de ce test de conformité fournisseur un blocage
inapproprié de la mission (`msg_14ff20a5b001rhUft92x8soYI1`, session racine).
L'utilisateur a demandé son retrait et la recherche des autres cas analogues
(`msg_14ff33e760013BF3r77xU7xSf2`).

Quatre contrôles ont été retirés : struct, refus bruts de bool/dvec3 et troncature
GL provoquée avec un buffer trop petit. Les préconditions des tests moteur et
les erreurs effectivement remontées par ARTIST sont restées. **Un seul test
découvert disparaît**, les autres retraits étant internes à des tests conservés.
Le détail et l'ancien FAIL préservé sont dans la
[[references/session-2026-10-18-attribute-driver-probes|note du retrait Q3]].

## Résultats historiques recoupés

Le lead de la session annonce le commit **f7bb171**, avec **326/326 PASS :
266 CPU + 60 OpenGL**, sans exclusion ni skip (`msg_1500211d6001SJSHxN2chyS1eF`).
Lors de cette ingestion, le lead a relu les artefacts Q3 :

- hashes des deux JUnit et de leurs logs conformes au manifeste ;
- 266 et 60 testcases XML, zéro failure/skip ; découverte 327 → 326 ;
- source AttributeComponentTests.cpp courante identique au hash enregistré :
  `768d0a318a960f4174bc5bea111daef56e07a2d383d509202150669bc14f9f96` ;
- ancien rapport de conformité driver toujours présent avec un failure.

**C'est un recoupement d'artefacts historiques, pas un nouveau run GPU.** Le
runner historique est le lead, non les reviewers. JUnit désigne ici le format
XML de rapport de GoogleTest/CTest, sans framework Java ni JVM ajoutés (E12,
`msg_150061b9f001pj3Zkaf6uDHrIJ`). Voir
[[development/build-and-test|les niveaux de validation]].

## Incidents et enseignements de méthode

- **Découverte GTest timeout 5 s** après compilation/link : le lead a annoncé une
  sonde directe `--gtest_list_tests` réussie en 60 ms, puis une seule relance,
  sans augmenter le délai. Ce n'était pas un RED moteur. Source E05, racine,
  `msg_14fe86bbe001ZhHIgHIRypiI2H` ; ce sous-run n'a pas été recontrôlé ici.
- **C1083 et chemin objet de 267 caractères** : le build-engineer proposait un
  diagnostic et une expérience de sortie courte, sans compilation effectuée par
  lui. La longueur seule n'établit pas la causalité MAX_PATH. Source E06,
  `ses_eb079ca5dffedl451uJly7M4HL`, `msg_14f98d08e001dPvpB7R8ptN63b`.
- **Texte QA, fichier commité et transport sont distincts.** Les restitutions
  lecture seule n'ont pas élargi les permissions. Un nouveau transport accessible
  ne prouve pas un correctif global d'enforcement. Sources E07, pool2
  `msg_14ecc836a001ntwPgJ0voVwcDF`, puis lead
  `msg_14f475cb9001L5wcbPpHhz7Rby`.
- **Fin d'auteur ≠ intégration.** Une reprise s'est arrêtée sur un mauvais HEAD
  sans mutation ; un autostash a bloqué team_merge avant application. Les dirty
  externes ne deviennent pas possédés. Sources E08, pool8
  `msg_14fd0c8f0001Xjz2QhgFmsXaP0`, puis lead
  `msg_14fd437b5001UZQCH8z51Yje40`. Le détail d'une restauration ITA/index n'est
  pas assez étayé ici pour devenir une procédure universelle.
- **Generator/reviewers distincts du runner.** G a proposé trois renforcements
  CPU ; le lead a annoncé leur intégration dans 4619395, sans changement de
  production, et des runs 10/10 Reader et 266/266 unit. La revue R n'avait pas
  exécuté de tests. Sources E09, pool9 `msg_14fde349c0015YkLXuhubTvWPD`, pool10
  `msg_14fe1f746001Y7PLbl1dO25d69`, lead `msg_14fe9f67a001ZBYzAs3V1q3q1S`.

## Garde-fous proposés, non adoptés

Les règles générales de séparation des preuves, TDD, ownership et intégration
existent déjà ; ne pas les recopier dans chaque agent.

Deux rappels ciblés méritent une éventuelle intégration canonique : nommer
l'obligation moteur avant de faire d'une sonde fournisseur un critère bloquant,
et choisir un oracle typé/discriminant lorsque la conversion intermédiaire perd
l'information à démontrer. ^[inferred]

Ces propositions sont tirées d'E02–E04 et de leurs contre-exemples. **Aucun fichier
d'agent, skill, permission ou règle opérationnelle n'est modifié par l'ingestion.**
Le dispositif initial d'ingestion a lui-même été réduit à une note et des
contrôles documentaires, après demande utilisateur de proportionner le travail.

## Ce qui reste ouvert

À la borne historique, le lead dit explicitement que **la clôture globale
Attribute-GL reste à faire** : audit final des tests de toute la mission,
recoupement du contrat des 12 formes et photographie documentaire finale
(`msg_150050af3001HidRIfDRpemjux`). La revue Q3 ne portait que sur le retrait.
Cette note ingère la session ; elle ne ferme pas ces obligations moteur.

ADR-0003/0005 restent Accepted/partial ; ADR-0008 est vérifié Windows Debug
seulement ; ADR-0001/2/4/6/7 restent Proposed. RAII exhaustive, lifetime VBO,
ASan, Release, autres OS/API, image et performance ne sont pas démontrés ici.
