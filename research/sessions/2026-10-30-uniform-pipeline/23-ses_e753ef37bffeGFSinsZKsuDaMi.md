# pool-12 (@engine-architect teammate)

Session: ses_e753ef37bffeGFSinsZKsuDaMi
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T03:01:54.764Z — msg_18ac1bbc9001RRdmMF4ebazEHa

## Décision locale proposée — à ratifier par le lead

Lecture seule réalisée ; aucun fichier modifié, processus lancé ou test exécuté. Pas de nouvelle ADR nécessaire pour ces deux corrections privées. Les statuts ADR existants restent inchangés.

### Faits et écarts

**R1 — bloquant confirmé par inspection.**
- `scripts/verification/runner.py:54–82` utilise `PIPE` et `communicate()`, puis deux tentatives de drainage.
- CPython 3.13 Windows, `C:/Python313/Lib/subprocess.py:1608–1644` : chaque thread attend `fh.read()` jusqu’à EOF ; `TimeoutExpired` est levé sans joindre les buffers déjà collectés.
- `runner.py:107–114` abandonne explicitement la fermeture des streams dont le reader est vivant. Le délai de retour ne garantit donc ni récupération des sorties disponibles, ni absence de thread/pipe conservé.
- `evidence.py:289–318` matérialise les logs seulement après retour du runner : des sorties perdues deviennent des logs vides persistés.

**R2 — incohérence confirmée, portée historique à préserver.**
- `evidence._build_files():362–377` exige exactement un exécutable non vide par cible requise et inventorie certaines configurations/bibliothèques.
- `reporting.report():134–146` recherche les nouvelles sources, mais ne réénumère pas cette population de build.
- Une copie supplémentaire peut donc laisser le rapport `CURRENT` alors qu’un nouveau `run` refuse l’ambiguïté. Cela n’invalide pas rétroactivement le binaire effectivement utilisé ni son résultat brut.

Le contrat ratifié, §§4/8, exige sorties disponibles conservées et fraîcheur vérifiée. Il ne définit pas littéralement le cas de la copie supplémentaire : R2 demande une clarification locale, pas une prétendue obligation déjà explicite.

## Contrat proposé

### R1 — fichiers temporaires plutôt que pompes de pipes

**Choix minimal :** rediriger stdout et stderr vers deux fichiers temporaires privés ; attendre uniquement le processus primaire, puis lire un snapshot fini de chaque fichier.

Dans `runner.py`, conserver `ProcessRunner.run()` et `ProcessOutcome` ; introduire au besoin trois helpers privés :
- `_capture_streams()` : acquisition et fermeture des fichiers ;
- `_stop_and_reap(child, deadline)` : arrêt du primaire possédé ;
- `_read_snapshot(reader, byte_count)` : lecture de la taille observée, sans attente d’EOF.

**Invariants :**
1. Aucun `PIPE`, thread lecteur ou `communicate()` pour cette capture.
2. Timeout/interruption : arrêt du primaire, puis `wait()` à échéance bornée ; aucun kill de groupe ou descendant tiers.
3. Les deux streams sont récupérés indépendamment, y compris si un seul continue à recevoir des écritures.
4. Après arrêt/récolte du primaire, relever une taille par fichier ; lire exactement ce préfixe via un **descripteur de lecture indépendant**. `dup()` ne suffit pas : il partage la position du fichier.
5. Les octets capturés sont restitués sans troncature silencieuse ; conserver le décodage réversible `surrogateescape`.
6. Toutes les ressources du parent sont fermées avant retour. Les snapshots persistés par `EvidenceSink.command()` ne restent pas liés aux fichiers encore inscriptibles.

**Windows :** choisir une ouverture temporaire avec partage de suppression, notamment `NamedTemporaryFile(delete=True)` et réouverture de lecture avec `os.O_TEMPORARY`. Vérifier cette combinaison par test réel factice sous CPython Windows. Ne pas remplacer par un fichier nommé dont l’effacement échoué serait simplement ignoré.

**Descendants :** un descendant peut conserver son handle hérité et continuer à écrire. Il n’est ni attendu ni tué. Sur Windows, le stockage temporaire peut subsister jusqu’à fermeture du dernier handle, avec suppression automatique ; ce n’est pas une garantie de nettoyage global des descendants. Le parent ne conserve aucun lecteur/thread abandonné.

**Limites explicites :**
- Les sorties conservées sont les octets rendus visibles dans les fichiers au snapshot ; pas les buffers utilisateurs non vidés ni les écritures ultérieures.
- Éviter toute promesse de délai absolu face à une panne du système de fichiers ou à un `kill`/reap défaillant. Un reap non confirmé doit produire une panne explicite, jamais un nettoyage annoncé réussi.
- Sans quota supplémentaire, le volume disque/mémoire reste proportionnel aux sorties. Introduire une limite serait un autre contrat, avec signalement explicite des dépassements.

**Alternative écartée :** récupérer `_stdout_buff`/`_stderr_buff` corrigerait une partie des pertes, mais dépendrait d’internals CPython sans supprimer les lecteurs bloqués. Des pompes annulables multi-OS seraient nettement plus complexes.

### R2 — même population fermée pour `run` et `report`

**Choix recommandé :** `CURRENT` signifie également que la **population pertinente actuellement découverte** correspond à celle acceptée avant exécution :
- mêmes cibles requises, chacune résolue vers un unique chemin non vide ;
- mêmes chemins et empreintes pour les configurations/bibliothèques déjà sélectionnées par `_build_files()`;
- aucune surveillance générale de tous les fichiers générés.

Extraire dans `evidence.py` un helper pur, par exemple `_collect_build_files(source_root, plan)`. `_build_files(sink)` peut rester un wrapper ; `reporting.report()` réutilise le même collecteur, sans subprocess, écriture ou observation Git supplémentaire.

Le rapport compare :
- le mapping cible → chemin avec `BuildReceipt.details["executables"]` ;
- l’ensemble sources + fichiers de build pertinents et leurs empreintes avec l’identité acceptée.

Ambiguïté, ajout/suppression pertinente ou changement : `STALE`, cause explicite et code d’identité existant. Une observation impossible ne peut pas produire `CURRENT`. Les résultats bruts historiques restent inchangés ; leur qualification actuelle suit les règles existantes du rapport.

**Alternative plus simple :** définir `CURRENT` comme « seuls les fichiers autrefois utilisés sont inchangés ». Défendable historiquement, mais incompatible avec l’objectif retenu de cohérence avec l’admissibilité actuelle de `run`.

## Validation discriminante proposée

**R1, programmes factices seulement :**
- Primaire écrit/flush stdout et stderr ; descendant conserve stdout ; timeout du primaire. Vérifier les deux marqueurs, retour borné avec marge, primaire récolté, absence de reader thread/pipe parent.
- Même cas avec **sortie normale du primaire avant fermeture du descendant** : aucun blocage sur EOF.
- Inversion stdout/stderr, interruption, sortie non UTF-8 et sortie dépassant la capacité usuelle d’un pipe.
- Le test possède aussi le descendant et le nettoie dans son `finally` ; le runner ne le tue pas.
- Doubles : kill/reap défaillant et vérification de la fermeture des ressources acquises.

**R2, arbre entièrement synthétique :**
1. Bundle valide, puis copie byte-identique `second/ArtistUniformCpuWorkload[.exe]` : `report` et admission de `run` refusent la même ambiguïté.
2. Suppression de cette copie : retour à la population initiale, sans altération des résultats bruts.
3. Ajout d’un fichier `.cmake` ou bibliothèque déjà couvert par le filtre : `STALE`.
4. Ajout d’un fichier généré hors population, par exemple `.txt` : fraîcheur inchangée.
5. `report` reste sans subprocess ni écriture.

## Conséquences et limites

Résolution **runtime**, par commande pour la capture et par consultation pour la fraîcheur : les handles et fichiers n’existent pas aux étapes compile/link/cook. Aucun changement moteur, shader, backend ou plan de frame ; aucune variante C++ supplémentaire.

**Statut : proposition non implémentée, validation NON_EXÉCUTÉE.** Les lectures corroborent les mécanismes ; elles ne certifient ni le commit exact du worktree ni un comportement exécuté. Aucune permission supplémentaire nécessaire pour cette proposition. Le lead peut ratifier ces invariants puis confier le RED/GREEN synthétique aux implémentants ; consommateurs maintenus gelés jusque-là.

## assistant — 2026-10-30T03:03:28.673Z — msg_18ac32a9e001MmD8oyYyQ2FA9G

Complément intégré à la proposition R2 :

- **Preuve lead sur `0f751ce9`** : ajout de `owned-new-runtime.dll` ou d’une seconde copie de cible après bundle factice PASS → rapport `CURRENT`, code 0. Deux échecs contre l’oracle proposé `STALE`, code 4 ; zéro erreur/skip, aucun binaire chargé.
- Cela confirme l’écart de **fraîcheur actuelle**, sans rendre faux le résultat historique.
- Recommandation maintenue : `CURRENT` couvre la même population fermée de cibles/configurations/bibliothèques que l’admission de `run`, pas tous les fichiers générés.
- **Ratification du lead requise avant GREEN R2.** R1 reste corroboré par inspection CPython uniquement, sans expérience processus réelle.

Aucune nouvelle action ni exécution effectuée.

## assistant — 2026-10-30T03:05:21.034Z — msg_18ac4e185001aYmi1kPgAa3JH8

# Proposition autonome R1 + R2 — ratification demandée au lead

**Statut : proposition locale, non implémentée.** Aucun fichier modifié, test ou processus lancé pendant cette consultation. Aucun changement moteur/backend/contrat de mission. Pas de nouvelle ADR nécessaire.

## 1. Faits et écarts

### R1 — capture Windows : défaut bloquant

Dans `scripts/verification/runner.py` :
- `SubprocessRunner.run():54–82` utilise deux `PIPE`, `communicate()` et des tentatives de drainage temporisées ;
- `partial():45–51` ne conserve que les sorties exposées par l’exception ;
- `107–114` laisse ouverts les streams dont le thread lecteur est encore vivant.

Inspection de `C:/Python313/Lib/subprocess.py:1608–1644` :
- le reader exécute `fh.read()` jusqu’à EOF ;
- le timeout peut être levé sans exposer les buffers collectés ;
- un descendant conservant stdout peut empêcher la récupération de stderr pourtant déjà terminé.

`evidence.py:289–318` écrit les logs après le retour du runner : une perte à cette frontière devient une preuve durable vide. **Constat statique, pas expérience processus réelle.**

### R2 — fraîcheur : incohérence confirmée

- `evidence._build_files():362–377` impose un exécutable unique non vide par cible requise et inventorie certaines configurations/bibliothèques.
- `reporting.report():134–146` ne redécouvre que les sources, puis vérifie les fichiers précédemment inventoriés : les nouveaux fichiers pertinents de build lui échappent.
- Preuve fournie par le lead sur **`0f751ce9`** : après bundle factice PASS, ajout de `owned-new-runtime.dll` ou d’une copie supplémentaire de cible → `CURRENT`, code 0. **Deux échecs contre l’oracle proposé `STALE`/4, zéro erreur/skip, aucun binaire chargé.**

Cela ne rend pas faux le résultat historique : le binaire utilisé et les résultats bruts n’ont pas changé.

## 2. R1 — stratégie recommandée

### Cible

Remplacer les pipes par **deux fichiers temporaires privés directement transmis à `Popen`**. Attendre le primaire, puis capturer un snapshot fini des fichiers. Aucun thread lecteur, aucun drainage jusqu’à EOF.

Conserver les interfaces publiques :
```text
ProcessRunner.run(command) -> ProcessOutcome
SubprocessRunner(popen_factory=...)
```

Helpers privés possibles dans `runner.py` :
```text
_capture_streams()
_stop_and_reap(child, deadline)
_read_snapshot(reader, byte_count)
```

### Ownership et transmission

- Le runner possède les fichiers temporaires, leurs handles parent et l’objet `Popen`.
- Pour chaque stream : un handle d’écriture transmis comme argument `stdout=`/`stderr=` à `popen_factory`, plus un **handle de lecture ouvert séparément**.
- Ne pas employer `os.dup()` pour fabriquer ce lecteur : sa position peut être partagée avec celle utilisée par les écrivains.
- Le child reçoit les handles standard par les mécanismes ordinaires de `Popen`, sans `shell=True` ni héritage élargi.
- `stdin=DEVNULL` reste inchangé.
- Tous les handles parent sont fermés avant retour, succès comme échec.

Sous Windows, employer une ouverture temporaire compatible avec le partage de suppression : notamment `NamedTemporaryFile(delete=True)` et réouverture du lecteur avec `os.O_TEMPORARY`. Vérifier cette combinaison dans un test factice Windows. Sous Linux, suppression automatique/unlink avec fermeture des descripteurs.

**Pas de simple fichier nommé dont un `PermissionError` de suppression serait ignoré.**

### Attente et snapshot

1. Lancer le primaire avec les deux fichiers comme sorties.
2. `wait(timeout=command.timeout_seconds)` : pas de `communicate()`.
3. Timeout : marquer `timed_out`, tuer le primaire possédé s’il est encore vivant, puis attendre sa récolte avec échéance finie.
4. Interruption : marquer `interrupted`, arrêter/récolter le primaire avec la même discipline bornée ; éviter une succession illimitée de délais réinitialisés.
5. Après récolte, relever séparément la taille de chaque fichier.
6. Lire exactement les préfixes ainsi définis, sans attendre EOF et sans poursuivre les écritures ultérieures.
7. Décoder avec `surrogateescape`, comme aujourd’hui ; `EvidenceSink.command()` persiste les snapshots dans les logs du bundle.

Les sorties « disponibles » désignent ici les octets visibles dans les fichiers aux instants de snapshot. Les buffers utilisateurs non vidés et les écritures ultérieures ne sont pas promis. Ne pas annoncer les logs comme complets jusqu’à la mort de tous les descendants.

### Descendants : limite explicite

Un descendant peut conserver un handle hérité et continuer à écrire :
- il n’est ni attendu ni tué par ce runner ;
- il ne bloque pas sur EOF le parent ;
- aucun thread/pipe parent n’est abandonné ;
- le stockage temporaire peut rester vivant jusqu’à fermeture du dernier handle hérité, avec suppression automatique appropriée.

Cela **ne garantit pas le nettoyage global des descendants**. Le descendant possède encore son handle ; le parent ne conserve pas une tâche de nettoyage oubliée.

### Erreurs : ne pas les transformer en succès

Cas à traiter :
- création/réouverture temporaire impossible : pas de lancement ;
- échec de lancement : fermeture de tout ce qui a été acquis ;
- disque plein/lecture défaillante : capture signalée comme défaillante, jamais journal silencieusement complet ;
- timeout/interruption : tenter les snapshots disponibles après arrêt/récolte ;
- reap non confirmé : panne explicite, jamais annonce de nettoyage réussi.

Une erreur de capture ne doit pas être seulement ajoutée à stderr avec un code enfant 0 : `_check_child()` pourrait accepter ce résultat.

**Adaptation privée recommandée :**
- une exception interne de capture transportant le `ProcessOutcome` disponible et sa cause ;
- `EvidenceSink.command()` enregistre d’abord le code enfant, les flags et les sorties récupérées, puis propage une `EvidenceError` de pipeline ;
- ne pas remplacer le véritable code enfant par un code synthétique de capture.

Si l’écriture même du bundle échoue, conserver une panne de persistance ; aucun receipt favorable ne doit être fabriqué.

### Bornes et limites

Cette solution borne l’attente du processus et supprime la dépendance à l’EOF des descendants. Elle ne garantit pas un délai absolu sous panne du système de fichiers ou impossibilité de récolter le primaire.

Sans nouveau quota, l’espace disque et la mémoire restent proportionnels aux sorties capturées. Un plafond en octets serait un contrat supplémentaire : aucun plafond silencieux proposé ici.

**Alternative écartée :** lire `_stdout_buff`/`_stderr_buff` corrigerait certaines pertes, mais resterait dépendant des internals CPython et laisserait les readers bloqués. Des pompes annulables spécifiques aux OS augmenteraient fortement la complexité.

## 3. R2 — définition locale de `CURRENT`

### Décision recommandée

`CURRENT` exige que la population pertinente actuellement découverte corresponde à celle acceptée avant exécution :
- même ensemble de cibles requises par le plan ;
- exactement un exécutable non vide par cible ;
- même mapping cible → chemin ;
- mêmes chemins et empreintes pour les configurations/bibliothèques **déjà sélectionnées par le filtre actuel** de `_build_files()` ;
- sources et métadonnées de révision toujours contrôlées comme aujourd’hui.

Cela ne surveille **pas tous les fichiers générés**.

Une copie byte-identique supplémentaire reste une ambiguïté de sélection : `STALE`, code 4. Le résultat historique demeure conservé et attribué au binaire effectivement utilisé.

### API privée et fichiers

Dans `evidence.py`, extraire :
```text
_collect_build_files(source_root, plan) -> (paths, executables)
```

- `_build_files(sink)` devient un wrapper compatible.
- `reporting.report()` appelle le même collecteur, sans construire artificiellement un sink à effets.
- Comparer le mapping avec `BuildReceipt.details["executables"]` et la population/les empreintes avec l’identité acceptée.
- Pas de subprocess, écriture, Git supplémentaire ou nouveau schéma requis.

Ambiguïté ou différence observée : `STALE`. Observation impossible : jamais `CURRENT`, avec diagnostic explicite selon les règles existantes. Avant build, ne pas exiger une population d’exécutables encore inexistante.

**Alternative plus simple :** `CURRENT` signifie seulement « les fichiers utilisés autrefois sont inchangés ». Possible, mais abandonne la cohérence demandée avec l’admissibilité actuelle de `run`. Je ne la recommande pas.

## 4. Tests RED discriminants et adaptations

### R1 — tests factices

1. Primaire flush stdout et stderr ; descendant conserve stdout ; timeout du primaire. Vérifier les deux marqueurs, délai avec marge, primaire récolté, aucune pompe/pipe parent.
2. Primaire termine normalement, descendant conserve stdout : retour sans attendre EOF.
3. Inverser les streams.
4. Interruption ; lancement impossible ; création/réouverture/lecture temporaire impossible ; reap défaillant.
5. Sortie non UTF-8 et volume supérieur à la capacité usuelle d’un pipe ; snapshots persistés sans perte silencieuse.
6. Erreur de capture avec code enfant 0 : pipeline en panne, véritable code enfant conservé.
7. Écriture tardive du descendant : logs persistés inchangés après retour.

Les tests possèdent les descendants factices et les nettoient en `finally`. **Le runner ne doit pas les tuer.**

### Impact sur les 132 tests existants

Je n’ai pas exécuté ni réinventorié ces 132 tests : aucun verdict global proposé.

Adaptations justifiées des seuls doubles concernés :
- remplacer les scénarios internes `communicate()` par `wait()/kill()/terminate()/poll()` ;
- les `popen_factory` factices acceptent de vrais objets fichiers pour `stdout`/`stderr`, y écrivent les octets prévus puis les flushent ;
- préserver les assertions métier : code retour, timeout, interruption, sorties, journaux et absence d’effets de `list/plan` ;
- ne pas conserver une assertion exigeant `PIPE` ou des threads : ce sont précisément les mécanismes remplacés ;
- ne pas affaiblir les tests de perte de sortie : ils doivent vérifier les octets effectivement persistés ;
- les runners injectés retournant directement `ProcessOutcome` restent compatibles.

Les doubles ne prouvent pas le comportement d’héritage Windows : il faut les tests avec programmes factices sur la plateforme concernée.

### R2 — tests synthétiques

- Réutiliser les deux RED du lead ; pas besoin d’une nouvelle infrastructure.
- Suppression de la copie ajoutée : retour à la population initiale.
- Ajout/suppression/changement d’une configuration ou bibliothèque couverte : `STALE`.
- Ajout d’un `.txt` hors filtre : fraîcheur inchangée.
- Mutation/suppression de l’exécutable original : toujours refusée.
- Rapport sans build : pas de nouvelle exigence prématurée.
- `report` : aucun subprocess ni écriture.
- Résultats bruts et attribution historique inchangés après constat `STALE`.

## 5. Validation et autorité

Choix runtime, par commande pour la capture et par consultation pour la fraîcheur. Aucun nouveau backend, framework, type C++, shader ou mécanisme GPU.

**À ratifier :**
1. capture directe vers fichiers temporaires, snapshots finis, ownership décrit ci-dessus ;
2. définition de `CURRENT` par population fermée commune à `run` et `report`.

Ensuite : implémentants test-first, RED/GREEN synthétique puis non-régression des suites du pipeline. Aucun moteur/build/GL/benchmark/cloud. Windows et Linux restent **NON_EXÉCUTÉS** jusqu’aux preuves respectives.

Aucune permission supplémentaire nécessaire pour cette proposition. Les consommateurs restent gelés jusqu’à ratification et intégration par le lead.