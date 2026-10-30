# pool-8 (@correctness-reviewer teammate)

Session: ses_e756c80f4ffepaJIo7YRI41V26
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T02:11:59.051Z — msg_18a9405c80010hajQHAO1D82Ty

Je vérifie le commit annoncé, puis les frontières processus, preuves et rapport. Serena pointe sur le dépôt canonique, pas sur ce worktree Python non intégré : je lirai donc les fichiers du candidat directement, sans exécuter de test ni de workload.

## assistant — 2026-10-30T02:17:49.339Z — msg_18a995e18001m3IG2zt0ndJ5iA

## Verdict

**Intégration déconseillée avant correction de quatre défauts établis par analyse statique.**

Candidat examiné : `2631e5cab88dbd4e0af713a6f8458356c6ba55b2`, worktree annoncé, propre lors du contrôle. Contrats et fixtures consultés. **Aucun fichier modifié ; aucun test, helper, build ou workload exécuté.** Les vérifications proposées ci-dessous restent NON_EXECUTE.

Tous les chemins sont relatifs au worktree candidat.

### 1. P1 — Drainage sans borne après timeout ou interruption

**Localisation :** `scripts/verification/runner.py:41`, `SubprocessRunner.run` ; également lignes 53 et 61.

- **Déclencheur :** le processus factice lance un descendant héritant de stdout/stderr. Ce descendant conserve les pipes ouverts après l’arrêt du processus directement possédé.
- **Attendu :** timeout/interruption borné, enfant possédé arrêté et récolté, sorties partielles conservées.
- **Observé statiquement :** après `kill()`, `communicate()` est rappelé **sans timeout**. L’EOF dépend encore du descendant ; le runner peut rester bloqué indéfiniment malgré le timeout déclaré.
- **Impact :** phase immobilisée, verrous bundle/build conservés, aucun receipt final.
- **Vérification minimale :** programme factice parent/descendant entièrement possédé par la fixture ; le descendant conserve les pipes au-delà du timeout. Vérifier une restitution bornée et le nettoyage explicite de la fixture, sans arrêt de processus tiers.

### 2. P1 — Écriture extérieure possible par symlink de `commands.jsonl`

**Localisation :** `scripts/verification/evidence.py:286`, `BundleSink.command` ; admission à `:430`, empreinte seulement à `:466`.

- **Déclencheur :** après une préparation valide, remplacer `bundle/commands.jsonl` par un symlink vers une copie extérieure du journal, puis demander `build`.
- **Attendu :** rejet avant lancement et avant toute écriture extérieure.
- **Observé statiquement :** le journal n’appartient pas encore aux artefacts du receipt de préparation. `_predecessor` ne le vérifie donc pas. `command()` le lit directement puis l’ouvre en mode append, sans contrôle `_within`.
- **Impact :** modification d’un fichier hors bundle ; la construction peut même terminer normalement. Le contrôle effectué à la fin de `run` arrive trop tard.
- **Vérification minimale :** utiliser uniquement `RuntimeFixture` et ses doubles ; conserver les octets du fichier extérieur, installer ce symlink, appeler `build`. Exiger rejet préalable, zéro appel runner et fichier extérieur inchangé.

### 3. P1 — Rapport favorable malgré contradiction entre empreintes du manifeste et du receipt

**Localisation :** `scripts/verification/reporting.py:118–125`, `report` ; reconstruction à `_completed_proof`, lignes 63–78.

- **Déclencheur :** partir d’un bundle synthétique PASS ; remplacer un résultat brut par du JSON tronqué et actualiser **uniquement** son empreinte dans `manifest.artifacts`. Laisser l’empreinte originale dans `receipts.run.artifacts`.
- **Attendu :** rejet de cette contradiction interne, jamais PASS.
- **Observé statiquement :** les receipts sont désérialisés, mais leurs empreintes ne sont comparées ni aux fichiers ni à l’inventaire global. Celui-ci accepte les nouveaux octets ; `_completed_proof` vérifie seulement leur présence dans l’inventaire, sans relire leur protocole.
- **Impact :** `report` peut conserver `COMPLETED/PASS/CURRENT`, code 0, alors que le résultat brut est désormais malformé et contredit le receipt.
- **Vérification minimale :** appliquer exactement ces deux mutations à une fixture terminée, puis appeler seulement `report`. Attendre code 4 et aucune écriture. Il ne s’agit pas d’exiger une protection cryptographique contre une réécriture totale, mais de rejeter des preuves déjà contradictoires.

### 4. P2 — Ajouts de fichiers pertinents ignorés par la fraîcheur du rapport

**Localisation :** `scripts/verification/reporting.py:135–140`, `report`.

- **Déclencheur :** après une exécution synthétique valide, ajouter un `CMakeUserPresets.json` auparavant absent, sans changer aucun fichier existant ni référence Git.
- **Attendu :** candidat déclaré STALE, conformément au contrôle des configurations et fichiers pertinents non suivis.
- **Observé statiquement :** le rapport parcourt uniquement les chemins de l’ancien snapshot. Il ne compare pas l’ensemble courant fourni par `_source_files`. Les métadonnées Git consultées ne détectent pas cet ajout.
- **Impact :** rapport favorable et `CURRENT` pour un ensemble source/configuration devenu différent.
- **Vérification minimale :** terminer la fixture, ajouter ce fichier, appeler `report` ; exiger STALE/non-zéro tout en conservant le mode lecture seule.

## Question distincte — raccordement des exécutables

**`scripts/verification/evidence.py:562`, `_execution_requests` :** les paramètres sont réconciliés, mais pas `command.argv[0]` avec les exécutables empreintés du `BuildReceipt`. Un autre binaire fournissant le même protocole pourrait donc être attribué au candidat.

Ce n’est **pas un défaut démontré d’un adaptateur de production**, absent de ce candidat. **DEFER_TO: engine-architect** pour préciser au parent la responsabilité de cette liaison lors du raccordement.

## Limites

Serena est attaché au dépôt canonique, avec serveur C++ ; il ne représente pas ce worktree Python non intégré. Analyse effectuée sur ses fichiers exacts. Les résultats des 125 tests annoncés ne sont pas validés par cette revue. Aucun constat de fonctionnement Windows/Linux, graphique ou de performance n’est revendiqué. Cette revue n’est pas exhaustive.

## assistant — 2026-10-30T02:55:21.566Z — msg_18abbbbda001mvYCiG5GlcAdac

## Ré-audit de `8ecb5675825b551ee6beede171ecf48e8351f48b`

Diff depuis `797cc568` examiné ; worktree propre. Tests/fixtures Python inchangés dans ce delta. **Lecture seule : aucune exécution de test, helper, build ou workload.** Deux résidus empêchent une levée complète.

### R1 — P1, bloquant : lecteurs et flux Windows peuvent survivre indéfiniment au runner

**Localisation :** `scripts/verification/runner.py:107–116`, `SubprocessRunner.run` ; restitution des sorties à `:45–51`.

**Scénario discriminant :** un programme factice produit des sorties puis lance un descendant conservant les pipes ouverts. Le processus primaire est arrêté, mais l’EOF n’arrive pas.

- **Corrigé :** les appels de drainage et `wait()` ont désormais des délais et un nombre d’essais borné. L’attente indéfinie initiale n’est plus présente dans ces appels.
- **Résidu statiquement établi :** sous Windows, le nettoyage **saute explicitement la fermeture** lorsque `stdout_thread` ou `stderr_thread` est vivant. Ces lecteurs et leurs pipes restent alors dépendants de l’EOF du descendant, potentiellement sans fin.
- **Perte supplémentaire de sorties :** dans le CPython 3.13 local, `subprocess.py:1608–1644`, les lecteurs font `fh.read()` jusqu’à EOF ; le timeout Windows est levé sans fournir les sorties partielles. Le helper `partial()` ne récupère donc pas les octets déjà produits. Le runner peut retourner des logs vides, puis laisser les lecteurs poursuivre leur consommation sans propriétaire chargé de les récolter.
- **Attendu :** restitution bornée **et** cycle de vie maîtrisé des lecteurs/flux, avec conservation des sorties disponibles.
- **Impact :** fuite de lecteurs et de handles ; sorties perdues ; mémoire potentiellement croissante si un descendant continue d’émettre après le retour.
- **Vérification minimale, NON_EXECUTE :** double reproduisant les exceptions Windows avec `stdout=None`, `stderr=None`, lecteurs vivants et flux ouverts. Vérifier fermeture/arrêt maîtrisés et conservation d’un marqueur produit. Le test actuel `test_review_regressions.py:52` fournit toujours des buffers dans `TimeoutExpired` et n’expose aucun lecteur : il ne discrimine pas ce résidu Windows.

**DEFER_TO: engine-architect** pour la stratégie de capture et d’arrêt des lecteurs respectant l’ownership, sans arrêt de processus tiers.

### R2 — P2, bloquant pour la fraîcheur : ajouts de binaires pertinents encore ignorés par `report`

**Localisation :** `scripts/verification/reporting.py:138–145`, `report` ; population correspondante dans `scripts/verification/evidence.py:362–369`, `_build_files`.

**Scénario discriminant :** après un bundle synthétique favorable, copier son exécutable requis dans un second sous-répertoire du même arbre de build, sous le même nom.

- **Attendu :** rapport non favorable/STALE pour cette population devenue différente et ambiguë.
- **Observé statiquement :** le nouveau contrôle énumère `_source_files`, qui exclut `out/build`. Il vérifie ensuite seulement les anciens chemins. Le rapport conserve donc les conclusions favorables, alors que `_build_files` rejetterait désormais les deux correspondances du target.
- **Impact :** divergence entre la fraîcheur annoncée par le rapport et les critères d’identité employés par les phases.
- **Vérification minimale, NON_EXECUTE :** fixture terminée, copie supplémentaire du target, puis appel exclusif à `report`. Exiger STALE/code 4 et absence d’écriture.

## Levées bornées des findings précédents

- **Symlink extérieur de `commands.jsonl` : levé statiquement pour le scénario signalé.** `evidence.py:263–287` contrôle le chemin, ouvre sans création/troncation, compare le handle au fichier courant et conserve ce handle pendant l’invocation et l’append. `_predecessor`, ligne 466, refuse aussi le chemin avant admission. **Le RED symlink reste SKIP sous Windows : aucune preuve exécutée de ce scénario n’est acquise ici.**
- **Contradiction receipt/index : levée statiquement.** `reporting.py:121–123` rejette désormais la réparation du seul index global, avant toute conclusion favorable.
- **Ajout de source/configuration : levé pour le témoin initial.** Un nouveau `CMakeUserPresets.json` est détecté par `reporting.py:138–140`. La réserve R2 concerne distinctement les fichiers de build.

## Compléments contrôlés

- Namespace Windows `-verification` présent dans `_build_root`.
- Refus du hardware CTest historique avant construction de l’adaptateur dans `verify.py:143–145`, et avant commandes/observations dans `execute`.
- Six variables réservées retirées de la copie de l’environnement parent, puis seules valeurs explicites réappliquées ; parent non modifié.

**Pas de verdict global PASS.** Les 132 synthétiques relèvent du runner lead ; adaptateurs et parser de production restent à composer. Les constats ci-dessus sont statiques, non une validation Windows/Linux exécutée ni une preuve exhaustive.

## assistant — 2026-10-30T02:56:21.517Z — msg_18abca6080011uoDNLDyW08G2k

Résultat attribué au lead : **132 découverts, 130 PASS, 2 SKIP symlink Windows**, zéro failure/error.

Il ne lève pas les deux résidus du ré-audit :
- **R1/P1** : lecteurs et pipes Windows pouvant survivre au retour, sorties partielles perdues.
- **R2/P2** : nouveaux binaires pertinents non détectés par `report`.

Candidat inchangé ; réception toujours réservée après correction/ré-audit ou arbitrage explicite du lead. Aucune nouvelle exécution de ma part.

## assistant — 2026-10-30T02:57:05.512Z — msg_18abd51e1001Ij4lH62bVaHwh7

## Remise autonome — résidus du candidat `8ecb5675825b551ee6beede171ecf48e8351f48b`

Analyse statique uniquement. Aucun test, processus factice, helper ou workload exécuté par moi. Les **130 PASS + 2 SKIP annoncés par le lead ne lèvent pas les points ci-dessous**.

## R1 — P1, bloquant : restitution bornée, mais lecteurs Windows abandonnés et sorties perdues

### Localisation

- `scripts/verification/runner.py:45–51`, helper local `partial`.
- `scripts/verification/runner.py:66–82`, `SubprocessRunner.run`, drainage borné.
- `scripts/verification/runner.py:107–116`, nettoyage des streams.
- `scripts/verification/evidence.py:306–309`, `BundleSink.command`, écriture des sorties retournées.

### Chemin discriminant concret

Sous Windows, le programme factice directement lancé :

1. écrit et flush des marqueurs distincts sur stdout et stderr ;
2. lance un descendant possédé par la fixture, conservant **stdout**, mais redirigeant son propre stderr ;
3. reste vivant jusqu’au timeout.

Après arrêt du primaire :
- le lecteur stderr peut terminer et conserver son marqueur ;
- le descendant garde stdout ouvert : son lecteur n’obtient pas EOF.

Le descendant est un témoin de fixture, **pas une autorisation de tuer des processus tiers**.

### Mécanisme exact

Le CPython 3.13 local, `C:/Python313/Lib/subprocess.py:1608–1644`, fournit le comportement pertinent :

- `_readerthread` appelle `fh.read()` jusqu’à EOF, puis seulement ajoute le résultat au buffer et ferme le flux ;
- `_communicate` attend d’abord le lecteur stdout ;
- si ce lecteur reste vivant, il lève `TimeoutExpired(self.args, orig_timeout)` **sans renseigner stdout/stderr dans l’exception**.

Conséquences dans le candidat :

1. `partial(error)` ne copie que `error.stdout` et `error.stderr` lorsqu’ils ne valent pas `None`. Ici, il ne récupère rien.
2. Les deux nouveaux essais de drainage expirent de la même manière.
3. Le nettoyage trouve `child.stdout_thread.is_alive()` vrai et exécute `continue`, ligne 114 : le pipe stdout n’est pas fermé.
4. Le thread reste vivant dans `fh.read()`. Son invocation conserve le `Popen` et le flux nécessaires à cette lecture ; leur libération dépend encore de l’EOF extérieur.
5. Même le marqueur stderr **déjà collecté dans `child._stderr_buff`** n’est pas récupéré : les appels à `communicate` expirent sur stdout avant de retourner les buffers.
6. Le `ProcessOutcome` est construit avec les variables locales restées vides, plus le diagnostic. `BundleSink.command` écrit ces seules chaînes dans les journaux.

Si l’EOF arrive ultérieurement, aucun chemin ne rattache ces données tardives aux logs déjà enregistrés.

### Attendu / observé / impact

- **Attendu :** retour borné, primaire récolté, cycle de vie maîtrisé des lecteurs/flux et conservation des sorties disponibles.
- **Observé statiquement :** le retour est désormais borné, mais le lecteur stdout et son pipe peuvent survivre indéfiniment ; des sorties produites, dont stderr déjà collecté, sont absentes des preuves.
- **Impact :** perte de diagnostic ; accumulation possible de threads/handles. Un descendant continuant d’émettre peut aussi faire croître la mémoire consommée par la lecture abandonnée.

La correction supprime donc l’attente indéfinie du chemin principal, **pas le défaut complet d’ownership et de conservation des sorties**.

### Obligation ratifiée et vérification minimale

La conservation des sorties partielles est déjà exigée par `contract.md:62,133` et le scénario QA S4. Ce n’est pas une extension demandant une campagne moteur ou une supervision générale des processus.

Vérification synthétique discriminante proposée, **NON_EXECUTE** : double reproduisant les exceptions Windows sans buffers, un lecteur stdout vivant, un buffer stderr déjà rempli et des streams observables. Le test actuel `test_review_regressions.py:52–109` ne couvre pas cela : il fournit toujours les sorties dans `TimeoutExpired` et n’expose aucun lecteur/stream.

**DEFER_TO: engine-architect** pour choisir la stratégie de capture/nettoyage ; aucune exigence de destruction arbitraire des descendants n’est proposée.

## R2 — P2 : divergence entre population de build et fraîcheur annoncée par `report`

### Localisation

- `scripts/verification/evidence.py:362–369`, `_build_files`.
- `scripts/verification/reporting.py:138–145`, `report`.
- `scripts/verification/reporting.py:47–51`, `_completed_proof`.

### Condition exacte et chemin discriminant

Après un bundle synthétique favorable, le baseline contient par exemple :

`out/build/linux-debug/ArtistUniformCpuWorkload`

Ajouter une copie à :

`out/build/linux-debug/second/ArtistUniformCpuWorkload`

Sans modifier :
- l’exécutable original ;
- les sources ;
- les artefacts et receipts du bundle ;
- les références Git.

### Attendu / observé

- `_build_files` recherche récursivement le nom de chaque target requis. Il trouve maintenant **deux correspondances** et rejette la population comme ambiguë, ligne 368.
- `report` n’appelle pas cette énumération :
  - `_source_files` exclut `out/build` ;
  - le nouvel exécutable n’apparaît donc pas dans `current_sources` ;
  - les anciens fichiers ont toujours leurs anciennes empreintes ;
  - `_completed_proof` accepte toujours le chemin original contenu dans le receipt.
- Le rapport conserve ainsi `CURRENT`, les verdicts antérieurs et le code 0.

**Vérification minimale proposée, NON_EXECUTE :** fixture terminée, ajout de cette copie, puis appel exclusif à `report`. Comparer sa conclusion avec le refus déterministe de `_build_files`, sans exécuter aucun binaire.

### Portée contractuelle — distinction importante

**Constat établi :** les deux chemins utilisent des critères différents pour la population courante des exécutables requis.

**Obligations déjà ratifiées :** attribution des exécutables, contrôle de fraîcheur et refus des mutations pertinentes — contrat §8, QA E4/S6.

**Limite de mon finding :** l’exécutable effectivement utilisé n’a pas changé. Ce scénario **ne démontre ni un mauvais résultat du test passé, ni l’exécution d’un binaire étranger**. Le contrat ne dit pas littéralement qu’une copie supplémentaire d’un target doit, à elle seule, périmer un rapport historique.

Je borne donc mon avis initial : **P2 de cohérence de fraîcheur selon le domaine d’identité actuellement utilisé par `_build_files`**, avec arbitrage requis si le lead souhaite au contraire limiter `CURRENT` aux seuls fichiers effectivement attribués. Exiger le rejet de *tout* nouveau fichier généré serait une extension ; je ne le demande pas.

**DEFER_TO: engine-architect** pour cette définition étroite de fraîcheur. R1 reste un blocage indépendamment de cet arbitrage.

## assistant — 2026-10-30T03:40:12.124Z — msg_18ae4c9d7001uWC7DIsHsoatLz

## Ré-audit de `258a9cbc5523846b42e99a75d6facfc80e19ed2c`

**R2 levé statiquement ; ancien mécanisme pipes/lecteurs de R1 supprimé. Un résidu d’ownership P1 subsiste.** Aucun test ni workload exécuté par moi.

### P1 — Le primaire peut échapper à l’arrêt sur une interruption hors du `try` d’attente

**Localisation :** `scripts/verification/runner.py:83–91,126–165`, `SubprocessRunner.run`.

**Scénario discriminant :**
1. `popen_factory` retourne normalement ; le primaire est enregistré dans `child`.
2. Un `KeyboardInterrupt` survient ligne 89 ou 90, avant l’entrée dans le `try` d’attente ligne 91.
3. L’exception rejoint directement le handler extérieur lignes 126–127.

**Attendu :** tenter l’arrêt/récolte bornés du primaire déjà possédé, puis restituer l’interruption et les préfixes disponibles.

**Observé statiquement :** ce handler appelle seulement `record`. Le `finally` photographie et ferme les fichiers, mais ne supervise pas le primaire. Les seuls appels `kill`/`terminate`/`wait` sont dans le bloc désormais contourné. Aucun arrêt ni aucune récolte n’est donc tenté dans ce chemin.

Le même trou existe pour une interruption supplémentaire entre un handler d’attente et l’entrée dans la boucle d’arrêt.

**Impact :** le pipeline restitue une interruption explicite, mais son primaire peut continuer à travailler après ce retour. La fermeture des fichiers parent ne termine pas ce processus.

**Vérification minimale proposée, NON_EXECUTE :** utiliser un faux primaire, puis une injection ponctuelle de `KeyboardInterrupt` à la ligne 89, après affectation de `child`. Vérifier les appels d’arrêt/récolte bornés. Le chemin actuel atteint `_CaptureError` avec zéro appel à ces opérations. Aucun processus réel n’est nécessaire.

C’est l’application de l’ownership et du nettoyage déjà ratifiés, **pas une nouvelle fonctionnalité ou une exigence d’arrêt des descendants**. La supervision du primaire doit aussi couvrir les sorties exceptionnelles extérieures au `try` d’attente.

## Levées bornées

### Ancien R1 — Capture Windows

Le mécanisme défectueux antérieur est supprimé :

- deux fichiers temporaires distincts ;
- aucun pipe ni thread lecteur parent ;
- tailles relevées avant lectures de préfixes finis ;
- readers ouverts séparément, sans `dup` partageant la position d’écriture ;
- fermeture des readers/descriptors et writers tentée indépendamment ;
- erreurs de capture/nettoyage conservées même après un code natif 0.

Les chemins traitant `TimeoutExpired`, `OSError` et interruptions **à l’intérieur des appels protégés** utilisent des attentes et nombres d’essais bornés. La réserve P1 concerne leur contournement exceptionnel décrit ci-dessus.

### Propagation des erreurs de capture

`evidence.py:303–326`, `BundleSink.command`, récupère le `ProcessOutcome` de `_CaptureError`, écrit les sorties et la trace native, puis lève `EvidenceError`. La cause est ensuite enregistrée par les handlers de phase via `_cause`, avant persistance du receipt. Un code enfant 0 n’efface donc pas cette panne.

### R2 — Population fermée du build

**Levé statiquement conformément à `68703a6`.**

`reporting.py:139–149` réutilise maintenant `_build_files`, puis compare exactement la population courante au baseline :

- deuxième correspondance d’un target requis → ambiguïté, STALE/code 4 ;
- bibliothèque/configuration supplémentaire appartenant au domaine existant → population différente, STALE/code 4.

Cela n’étend pas le contrôle à tous les fichiers générés. Le chemin reste en lecture seule.

## Limites de preuve

Résultats attribués au lead : **9/9 ciblés**, puis **143 découverts, 141 PASS, 2 SKIP symlink Windows**. Ils ne couvrent pas l’interruption après acquisition du primaire et avant son bloc d’attente.

Aucune expérience réelle de descendant réalisée ; les deux scénarios symlink Windows restent INCONCLUSIVE. Aucun verdict global sur le pipeline ou ses consommateurs de production non composés.

## assistant — 2026-10-30T03:57:27.997Z — msg_18af49839001p1YO9Xz4tTB6Fh

## Ré-audit ciblé — `e1bef8ddef6e16b81408260daca594be00bcc1d5`

**Résidu P1 levé statiquement dans le périmètre ratifié. Aucun nouveau finding sur ce petit diff.**

- `runner.py:48–49` : `child` et `reaped` sont initialisés avant acquisition.
- `runner.py:101–131` : l’arrêt/récolte appartient désormais au `finally` englobant. L’interruption après acquisition, avant l’attente protégée, rejoint donc ce nettoyage.
- La sortie du handler d’attente rejoint également ce `finally` : l’ancienne fenêtre extérieure précédant la boucle d’arrêt est supprimée.
- Deux tentatives maximum, attentes finies, erreurs conservées ; aucun signalement ni attente des descendants.
- `runner.py:133–168` : snapshots et fermetures suivent les tentatives de récolte ; les fichiers parent restent protégés par leur propre `finally`.
- Un primaire déjà récolté n’est pas arrêté une seconde fois.

Le test AST inchangé cible effectivement la frontière après acquisition et conserve ses assertions d’arrêt, récolte, sorties natives, interruption et fermeture avant cleanup de secours.

**Limite explicite :** nettoyage best-effort face aux interruptions pendant le nettoyage lui-même, sans promesse d’immunité absolue aux SIGINT répétés.

### Preuves et réception

Résultats attribués au lead : **1/1 régression**, puis **144 découverts, 142 PASS, 2 SKIP symlink Windows**. Aucun test exécuté par moi.

**Avis favorable à la réception des fondations du cœur**, sous ces limites. R2 et les audits antérieurs inchangés n’ont pas été rejoués. Les symlinks Windows restent INCONCLUSIVE ; cette réception ne valide ni la composition production/adaptateurs, ni CTest hardware, ni une exécution moteur/GL.