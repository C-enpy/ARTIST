# GREEN_READY — collecteur contexte historique et préflight explicite

## Candidat et attribution des phases

Worktree natif :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.
Branche : `ensemble-3b74f9ed-91ef-410f-8ac6-431ecb896a6d`.
Base de cette correction : `1101b64707e1f336c61794fc1ffbb394c721fa5b`.

Entrées reçues et RED exécutés par le **lead**, pas l'auteur :

- `19085a9ba1be1909be949eb74c96408930b6246e` : 20 méthodes, 33 failures de
  sous-cas, zéro error/skip, code1 ;
  `out/verification-synthetic/context-collector-red-2026-10-30T04-13-37-465Z/`.
- `1101b64707e1f336c61794fc1ffbb394c721fa5b` : trois méthodes, 13 failures de
  sous-cas, zéro error/skip ;
  `out/verification-synthetic/context-preflight-red-2026-10-30T04-18-50-495Z/`.
- Mapping et transfert de l'unique assertion ratifiés au lead dans
  `f4021cc48d421bab7a1a35dd3e55a88b0dd82016` (contrat et rapport75), confirmés
  par message natif. Ce commit documentaire n'a pas été copié/cherry-pické dans
  le worktree. Le droit d'édition provient du transfert explicite reçu.

La majorité des premiers RED étaient masqués par la garde inconditionnelle ;
leur sensibilité individuelle aval reste à vérifier sur ce GREEN et en revue.
**GREEN_READY signifie prêt au runner lead, pas GREEN observé.** Aucun test par
l'auteur ; aucun résultat de campagne, de build ou de rendu revendiqué.

## Cause et correction minimale

Le producteur/parser de contexte était intégré, mais l'exécution et la CLI
refusaient encore toute sélection historique hardware sans inspecter son
transport. La réconciliation historique rejetait également les skips avant de
pouvoir distinguer précondition structurée et assertion réelle.

### `scripts/verification/evidence.py`

- `_has_hardware_ctest` remplace le prédicat de refus inconditionnel ; aucune
  capacité publique ou disponibilité mise en cache.
- `_ctest_context_requests` inspecte une seule description **pure** avant toute
  nouvelle sonde de révision/identité ou enfant. Pour chaque JUnit : six variables
  explicites requises ; absence/omission →3 ; valeurs complètes étrangères,
  request_id invalide/dupliqué ou chemin dupliqué →4. Run/hash/cible viennent du
  plan et du predecessor déjà vérifiés, pas de l'environnement du parent.
- `execute` conserve la garde cible absente avant toute description. Les autres
  profils gardent leur ordre d'admission acquis. Aucun prepare/build implicite,
  aucune observation device dans cette nouvelle étape.
- `_hardware_context_findings` appelle le vrai `parse_context_observation` intégré
  avec les sept clés, depuis le seul system-out du cas et l'identité de son
  invocation JUnit. Aucun parseur JSON concurrent, preuve globale, copier/coller
  du schéma workload ou qualification depuis un nom de GPU. Les cas unit.* ne
  requièrent pas d'observation GPU.
- `_junit` réutilise `linux-evidence.py::result_summary` pour les populations. Il
  distingue les skips structurés, collecte toutes les causes de contexte des cas
  et l'assertion indépendante, conserve population et retours natifs, puis applique
  la priorité 4/5/3 sans dépendance à l'ordre des cas. Les chemins d'interruption
  130 et de capture du runner restent inchangés. La fonction publique
  `reconcile_junit` conserve son rejet des skips ordinaires.
- Précondition attribuée + skip cohérent →3 ; contexte indisponible sans arrêt,
  skip ordinaire ou observation malformée/étrangère →4 ; vraie assertion →FAIL/5.
  Assertion + précondition conserve les deux causes et FAIL/5 ; assertion +
  protocole conserve les deux causes et FAIL/4. Les causes sont ordonnées de
  manière déterministe. Les bruts XML/stdout/stderr sont conservés avant analyse.
- La trace conserve les six variables publiques **explicitement déclarées**,
  nécessaires à la relecture ; l'allowlist de l'observation générale
  d'environnement n'est pas élargie. Aucun dump d'environnement/secret.

### `scripts/verification/reporting.py`

`_completed_proof` reconstruit les invocations hardware depuis les commandes
attribuées, la découverte brute et les XML originaux. Il réemploie le même
préflight et le même parser/interprétation par cas, contrôle la population et
refuse un verdict favorable contradictoire. Pas de cache de qualification,
sonde, processus, écriture ou reconstruction de preuve manquante. Les contrôles
d'identité/artefacts et les rapports non terminés restent en place.

### `scripts/verify.py`

Retrait de la seule garde « toute sélection historique est unsupported ». Cible
absente reste bloquée avant factory ; sinon le cœur inspecte la description
explicite de l'adaptateur composé. Le nominal CLI synthétique reçoit exclusivement
son factory/runner factices. Les adaptateurs de production encore absents ne sont
pas substitués, copiés ni importés par une validation de cette tranche.

### Mapping acquis, strictement borné

Dans `test_review_regressions.py`, une seule ligne transférée :
`adapter.calls == [*adapter_calls, 'run']` au lieu d'aucun appel supplémentaire.
Code3/BLOCKED, zéro enfant, zéro nouvelle révision, zéro parser et aucune
observation device restent inchangés. Le diff exact de ce fichier a été relu.
Les 20+3 nouvelles méthodes ne sont pas modifiées ; aucune autre assertion
acquise n'est supprimée, affaiblie ou passée en skip.

## Ownership, durées de vie et erreurs

Le sink possède les fichiers ; le mapping privé d'attribution ne vit que pendant
la collecte et ne transporte aucun pointeur/emprunt/contexte GPU. Le parser reçoit
une chaîne du cas et retourne des données locales ; aucune preuve réutilisée d'un
autre cas/run/invocation. Les durées de vie GPU et le retrait natif restent chez
le producteur, sans nouvelle garantie dérivée du scope Python.

Le runner, ses handles/snapshots, le modèle public, les adaptateurs, le producteur
C++ et les schémas sont inchangés. Une panne de qualification ne donne pas un
receipt favorable ; une assertion déjà attribuée n'est pas remplacée par une
précondition découverte dans un autre cas. Un seul écrivain par bundle/build et
source stable pendant l'exécution : synchronisation/leases existantes conservées.

## Contrôles auteur et commandes lead

Contrôles statiques uniquement : parsing AST sans import/exécution des quatre
fichiers Python modifiés, syntaxe valide ; `git diff --check` code0 ; comparaison
contre `1101b64` vide pour les 20+3 tests ; diff de la régression limité à la seule
assertion ratifiée. Les callers privés ont été recherchés textuellement ; Serena
reste lié à ARTIST avec LSP C++ seulement, sans couverture symbolique Python.

Commandes proposées au lead, séquentiellement, dans ce worktree :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_collector_contract.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_preflight_contract.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
```

Inventaire attendu selon la base : 20+3 ciblés, 167 core, 28 parser ; ce sont des
nombres proposés à recouper à la découverte, pas des résultats ni constantes dans
le pipeline. Les deux symlinks Windows historiquement non exécutés ne deviennent
pas une preuve par ce diff. Le lead conserve les commandes/codes/logs/comptages
effectifs puis fait réauditer les risques modifiés indépendamment.

## Limites explicites

Tests auteur : **zéro**. GREEN et non-régression **NON_EXECUTE** ici. La compilation
C++, la garde native, GL, rendu et performance restent NON_COMPILE/NON_EXECUTE.
Pas de processus réel, CTest/CMake, moteur, benchmark, installation ou cloud par
l'auteur. Aucun changement d'adaptateur de production : leur raccordement demeure
une barrière distincte. Ce candidat est source pour qualification ultérieure,
jamais fonctionnement Windows/Linux matériel démontré.
