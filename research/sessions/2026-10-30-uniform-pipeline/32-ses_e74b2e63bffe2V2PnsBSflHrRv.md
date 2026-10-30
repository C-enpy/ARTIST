# pool-21 (@qa-planner teammate)

Session: ses_e74b2e63bffe2V2PnsBSflHrRv
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T05:34:58.807Z — msg_18b4ddef4001bn4XbXogv09VOP

# Complément proposé à `qa-plan.md` — raccordement bundle Windows

**Destination unique :** `.opencode/evidence/mission-uniform-pipeline/qa-plan.md`, addendum à persister et committer par le lead **avant le nouveau worktree auteur**. Aucun fichier modifié, aucun test exécuté ici.

## BLOCKED

Lecture de `.opencode/evidence/mission-uniform-pipeline/qa-inputs.md` : **`File not found: .opencode/evidence/mission-uniform-pipeline/qa-inputs.md`**. Aucune nouvelle tentative ni recherche de transport alternatif.

La digestion pool-5 référencée n’a donc pas été consultée. Le brief bundle, le binding ratifié, le plan existant et les sources locales permettent néanmoins le complément ci-dessous.

## 1. Pin observé et limites de preuve

- HEAD **fourni par le lead** : `493bf4f73a8ba5f8f1b7350deb7aaab09fcd1b3b` ; non vérifié par Git ici.
- **Code observé :** `adapters/base.py:PlatformAdapter.describe_preparation` et `evidence.py:prepare` utilisent encore `describe_preparation(plan)`. Le keyword `bundle_root` est un **delta futur**, pas une interface livrée.
- **Intention ratifiée :** `windows-adapter-binding-v1.md`, notamment lignes 28–53.
- Le registre ADR local, daté du 29 octobre, indique désormais `partial` pour l’implémentation de 0001/0002/0004/0007, toujours **Proposed**. Cela diverge du pin historique « not-started », sans adoption nouvelle. Ne pas réappliquer ce pin ancien ni modifier le profil.
- Les 170 PASS + 2 SKIP/172 core, 28 parser, 31 scénarios, fondations Windows 51 et Linux 6+19 sont des **exécutions attribuées au lead par le brief**, pas des exécutions de ce planner ni des preuves du delta.
- Lecture locale et envoi `team_message` réussis. Réception du candidat final à confirmer par le lead ; aucune garantie de sandbox déduite des permissions.

## 2. Obligations du lot

### Explicites

**B1 — Interface pure et bundle explicite.**  
`describe_preparation(plan, *, bundle_root: str)` reçoit le chemin absolu du bundle admis sous lease. Linux et doubles acceptent le keyword ; aucun fallback après `TypeError`.

**B2 — Transport privé possédé par le sink.**  
Création exclusive de `raw/windows-helper-input.json`, exactement :
`build_directory`, `targets`, `executable: ""`, `arguments: []`. `build_directory` suit `_build_root` ; `targets` est l’union ordonnée dédupliquée du plan. Fermeture, SHA256 et inscription aux artefacts précèdent toute commande.

**B3 — Réemploi sans réparation.**  
Build réutilise le payload du PreparationReceipt admis. Collision, disparition, mutation ou receipt étranger refusent avant lancement ; aucune réécriture réparatrice.

**B4 — Effets et phases bornés.**  
Descriptions/list/plan purs ; logs privés prepare/build créés exclusivement ; opt-in contrôlé avant commandes de préparation ; aucune construction/exécution implicite.

**B5 — Conservation et attribution.**  
Bruts et artefacts partiels conservés ; interruption/échec ne produisent aucun receipt favorable. Capture commune primaire, sans deuxième capture.

**B6 — Compatibilité.**  
Request, CommandSpec, schémas de receipts et signature build inchangés. Consommateurs adaptés seulement après intégration du core.

### Contrôles inférés, non nouvelles fonctionnalités

- **I1 :** sentinelles d’effets et compteur d’appels pour rendre les refus discriminants.
- **I2 :** injection d’interruptions aux frontières de publication, sans prétendre simuler toutes les pannes système.
- **I3 :** permutation de deux bundles du même plan pour détecter un état global ou « dernier bundle » mémorisé.

## 3. Top trois risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| Attribution au mauvais bundle ou réemploi d’un payload muté | 5 | 4 | **20** |
| Écriture/commande implicite ou collision avec un chemin réservé | 5 | 3 | **15** |
| Publication prématurée après interruption ou perte des bruts | 4 | 3 | **12** |

**R1 — Attribution/réemploi.**  
Déclencheur : deux bundles du même plan, receipt échangé, mutation entre prepare/build ou ancienne API masquée par fallback. Portée : commandes et preuves attribuées au mauvais candidat. Détection actuelle : `_predecessor` vérifie identités et empreintes ; source observée seulement. Mitigation actuelle : bundle exclusif, receipts liés au run/plan. **Gap :** payload Windows absent du flux actuel ; tests B1–B3 nécessaires.

**R2 — Effets/collisions.**  
Déclencheur : description matérialisant des fichiers, opt-in tardif, préexistence du payload/des logs, substitution d’un chemin interne réservé. Portée : preuves existantes, arborescence locale et interdictions de mission. Détection actuelle : tests de pureté présents ; confinement `_within`. Mitigation : lease et création exclusive du bundle. **Gap :** confinement/empreinte ne prouvent pas seuls l’ownership du nouveau transport ; refus no-clobber à démontrer.

**R3 — Publication/interruption.**  
Déclencheur : interruption pendant écriture, après fermeture mais avant hash, après hash mais avant commande, ou pendant fake command. Portée : validité du receipt et conservation des preuves. Détection actuelle : `_cause`, tests d’interruption et conservation présents. Mitigation : états distincts, receipts et logs existants. **Gap :** ordre exact de publication du nouveau payload non testé.

Réévaluer P à l’intégration et après tout contre-exemple ; ne pas modifier les scores PM/Ensemble.

## 4. Scénarios discriminants proposés

Tous sont **GAP** pour le delta jusqu’à preuve qualifiée. Tests synthétiques seulement, écrits par l’auteur et exécutés par le lead après relecture.

| Groupe → obligations | Nominal | Négatif et oracle |
|---|---|---|
| **T1 → B1, B6, I3** | Deux bundles séquentiels du même plan ; descriptions répétées, chemins distincts et propres à chaque bundle. Linux/doubles acceptent le keyword. | Ancien double ou double levant un `TypeError` interne : aucun second appel sans keyword, aucune commande ; pas de masquage causal. |
| **T2 → B2** | Fake runner ouvre/lit le payload dès son entrée ; vérifie quatre clés exactes, valeurs, hash et inscription déjà publiés. | Champ superflu, cible dupliquée/mal ordonnée ou mauvais build root : assertion discriminante. Vérifier la fermeture avant lancement, pas seulement le contenu après retour. |
| **T3 → B2, B3** | Build relit le même fichier et les mêmes octets depuis le receipt admis. | Mutation, suppression, receipt étranger ou incomplet : zéro appel adaptateur/runner lorsque l’admission échoue ; octets non réparés, aucun succès. |
| **T4 → B2, B4** | Payload et répertoires logs absents : création exclusive aux phases prévues. | Fichier/répertoire préexistant aux destinations ; redirection vers chemin réservé, dont `raw/ctest-population.json` : refus avant commande, contenu initial intact. Symlink seulement si fixture supportée ; skip explicite sinon. |
| **T5 → B1, B4** | Descriptions répétées et CLI list/plan gardent sorties déterministes et arbre inchangé. Prepare autorisé ne décrit/lance que prepare. | Opt-in absent : aucune commande helper/bootstrap/build/run. Sentinelles écriture/enfant/réseau déclenchées : échec du contrôle, jamais fallback réel. |
| **T6 → B2, B5, I2** | Prepare/build factices réussis : capture commune primaire et logs privés attribués. | Interruptions aux frontières de publication et fake enfant nonzero : pas de receipt favorable ; code causal, bruts et préfixes disponibles préservés ; nouvelle tentative dans nouveau bundle. |
| **T7 → B6** | Aller-retour des modèles existants et consommateurs adaptés après intégration. | Aucun nouveau champ public, schéma dupliqué, setter/global ou signature build modifiée ; audit de diff et contrats existants discriminants. |

**Passe heuristique :** états prepare/build admis/refusé/interrompu ; frontières absent/existant/partiel ; ordre fermeture→empreinte→publication→commande ; répétition pure des descriptions, mais phases non réparatrices ; concurrence limitée au lease existant, sans infrastructure nouvelle.

## 5. Registre obligations ↔ preuves

| Obligation | Localisation observée / raison | Preuve à rattacher |
|---|---|---|
| B1 | `adapters/base.py:PlatformAdapter.describe_preparation`, `evidence.py:prepare` : frontière actuelle sans keyword | T1, T5 |
| B2 | `evidence.py:BundleSink.artifact`, `_build_root`, `prepare` : calcul et publication des artefacts | T2, T4, T6 |
| B3 | `evidence.py:_predecessor`, `build` : admission run/plan/empreintes | T3 |
| B4 | `verify.py:main`, `BundleSink.lease` : séparation consultatif/runtime et ownership | T4, T5 |
| B5 | `evidence.py:_cause`, `_finish`, `prepare`, `build` : états et receipts | T6 |
| B6 | `adapters/base.py`, `runtime_fixtures.py:AdapterDouble`, modèles existants | T1, T7 |

**Sens inverse et réemploi :**

- `test_runtime_contract.py:test_invalid_predecessor_receipt_is_rejected_before_any_child_or_adapter_call` → B3 ; base à conserver, pas preuve du nouveau payload.
- `…:test_help_list_and_plan_never_load_adapters_probe_spawn_or_write` et `…:test_real_cli_entrypoint_cold_list_and_plan_have_no_import_or_bytecode_effects` → B4.
- `…:test_failed_or_interrupted_preparation_and_build_keep_partial_logs` → B5.
- `test_plan_contract.py:AdapterInterfaceTests` → B1/B6, adaptation bornée requise.

Ces références sont **static-presence** : corps non audités intégralement, aucune exécution ici. Les nouveaux tests doivent pointer B1–B6 ; aucun orphelin établi par cette inspection ciblée. Éviter un doublon faible qui vérifie seulement « fichier présent après commande » : T2 exige l’observation **avant** commande.

## 6. Budget proportionné

Pour **100 unités relatives additionnelles**, sans engagement horaire :

- Setup : **25** — réemploi des fixtures, payloads et fake runner.
- Planifié : **56,25** — T1/T3 : 16 ; T2/T4 : 17 ; T5 : 8 ; T6 : 10 ; T7/revue ciblée : 5,25.
- Buffer : **18,75** — investigation, panne de fixture, ré-audit.

Soit **75 % planifiés / 25 % buffer après setup**. Réduire cosmétique et extensions avant les risques ≥10 ; ne pas absorber le buffer pour conserver du moyen. Réestimer si ce budget devient irréalisable.

## 7. Preuves, gaps et sortie

Pour chaque RED/GREEN/non-régression retenu, le lead enregistre : **HEAD, delta, SHA256 des fichiers pertinents, source/date, commande/cwd, runner, interpréteur/environnement, attendu/observé, découverts/exécutés/échoués/skipped**. Un candidat différent rend la preuve antérieure périmée pour le delta, pas automatiquement inutilisable pour les fondations inchangées.

Les comptages transmis ne contiennent pas ici toutes ces métadonnées : les rattacher aux journaux existants, **sans refaire la campagne**. Aucun groupe du delta n’est actuellement COVERED.

**Gates de sortie :**

1. Complément persisté/committé avant auteur.
2. RED causal des nouveaux contrats, GREEN minimal, non-régression ciblée par runner lead ; zéro test ou skip requis ≠ PASS.
3. Core réellement intégré avant consommateurs Windows/Linux.
4. Revue fresh ciblée ownership/publication/pureté, puis correction et ré-audit des findings bloquants.
5. Aucun changement public non prévu, aucune réparation silencieuse, aucun brut perdu.

**GAP-defer proposé, à acter par le lead :** qualification Windows/Linux réelle, C++/OpenGL et performances ; propriétaire lead/futur responsable de qualification ; échéance avant toute revendication de fonctionnement sur la plateforme. Impact : livraison **pour qualification, NON_EXECUTE**, sans démonstration moteur/GPU.

**Décisions restantes :** acceptation/persistance du complément, rattachement des métadonnées historiques et confirmation de réception finale. Aucun nouveau choix d’architecture ni autorisation d’implémentation n’est demandé ou accordé par cette remise.

## assistant — 2026-10-30T05:36:16.544Z — msg_18b4f0e9c001OsTEjp7Ygsz0c5

Complément complet remis dans la réponse finale précédente, à intégrer dans `.opencode/evidence/mission-uniform-pipeline/qa-plan.md`.

**Rectification de la section BLOCKED :** `qa-inputs.md` absent est une erreur de référence du brief, confirmée par le lead, **pas un refus de permission ni un blocage du lot**. Aucun oracle du delta étroit prepare/bundle/payload/receipts n’en dépend : les sources lisibles et le plan existant suffisent.

Le reste du complément demeure inchangé : delta futur non livré, T1–T7 à qualifier, preuves historiques réutilisées sans nouvelle campagne, persistance/commit par le lead avant auteur. Aucun élargissement aux scénarios GPU, aucune édition ou exécution effectuée.

## assistant — 2026-10-30T06:56:45.154Z — msg_18b98bc5e001g38netiBsOaV31

## Addendum proposé à `qa-plan.md` — environnement enfant matériel

**Périmètre :** `scripts/verification/runner.py` et tests core uniquement. À persister/committer par le lead avant nouveau worktree auteur. Le précédent complément bundle reste acquis.

### 1. Sources et constat

- Contrat lu : `hardware-child-environment-v1.md`, commit **fourni par le lead** `7a5ed1632278add245a6d65b7f86875b60907b36`.
- Code observé : `runner.py:SubprocessRunner.run` copie actuellement le parent, retire les six variables de contexte, puis applique `env_delta`. Il ne retire pas les deux forçages logiciels.
- Test existant lu : `test_context_transport_contract.py:ContextTransportContractTests.test_inherited_public_context_is_removed_before_only_declared_delta_is_applied`. Il observe déjà l’environnement transmis à un faux `popen_factory`, sans processus enfant.
- Les **59 tests Linux PASS**, attribués au lead, ne prouvent pas cet héritage effectif. Candidat Linux `3e697590` annoncé non intégré.
- Aucun test, processus ou changement effectué ici ; aucun refus de lecture dans ce complément.

### 2. Obligations

**Explicites**

- **H1 :** déclenchement uniquement si le delta déclare exactement `ARTIST_VERIFICATION_CONTEXT_V1=opengl-hardware`, indépendamment d’argv.
- **H2 :** dans ce cas, six champs de contexte explicitement présents et non vides ; refus avant lancement sinon. Pas de nouveau validateur sémantique runner.
- **H3 :** retirer `LIBGL_ALWAYS_SOFTWARE` et `GALLIUM_DRIVER` de la copie enfant ; refuser leur présence dans le delta, même avec valeur vide ou `"0"`.
- **H4 :** contexte matériel seulement ambiant inactif ; contexte ambiant nettoyé ; comportement CPU/sans déclaration et logiciel explicite inchangé.
- **H5 :** parent intact, aucune contamination entre appels ; refus identifié comme panne du pipeline, jamais résultat favorable ou précondition GPU.
- **H6 :** aucun schéma modifié ; capture, causes et gestion du processus primaire conservées.

**Contrôles inférés**

- **I1 :** comparer une copie complète du parent avant/après chaque appel.
- **I2 :** faire échouer strictement le faux factory pour toute invocation inattendue et vérifier son compteur.
- **I3 :** permuter les profils sur une même instance runner pour détecter un état conservé.

### 3. Top risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| Forçage logiciel conservé malgré déclaration matérielle | 5 | 4 | **20** |
| Transport incomplet ou réintroduction acceptée | 4 | 3 | **12** |
| Assainissement débordant sur parent/profils suivants | 4 | 3 | **12** |

**R1 :** déclencheur : parent contenant l’un ou les deux forçages logiciels ; portée : workloads, découverte et lancement CTest matériels. Détection actuelle : finding indépendant transmis, source confirmant l’héritage. Mitigation actuelle : contrôle GL ultérieur, qui ne corrige pas l’environnement demandé. **Gap :** oracle sur l’environnement effectif au factory.

**R2 :** déclencheur : champ absent/vide ou forçage ajouté au delta ; portée : admission des commandes matérielles et attribution des preuves. Détection actuelle : contrat, pas de garde runner observée. Mitigation actuelle : admission sémantique au core, insuffisante pour ce nouveau contrat local. **Gap :** refus causal et zéro lancement à démontrer.

**R3 :** déclencheur : suppression dans `os.environ`, nettoyage inconditionnel ou réemploi d’un environnement précédent ; portée : appels CPU/logiciel suivants et processus parent. Détection actuelle : test existant de préservation partielle du parent. Mitigation actuelle : copie locale à chaque appel. **Gap :** matrice séquentielle et comparaison complète après succès/refus.

### 4. Tests ciblés et oracles

Réutiliser le fichier de transport et ses doubles ; aucun nouveau harness.

| Test proposé → obligations | Nominal | Négatif / oracle discriminant |
|---|---|---|
| **HT1 → H1, H3** | Delta matériel complet ; parent avec chacun des forçages puis les deux. Paramétrer argv workload, CTest et découverte. | Au factory : deux clés **absentes**, six valeurs exactement déclarées, variable ordinaire conservée. Ni `None`, ni chaîne vide de substitution. |
| **HT2 → H2, H5** | Six champs explicites non vides admis. | Retirer puis vider chacun des six champs ; fournir sa valeur dans le parent ne répare pas le delta. Refus causal avant factory, compteur zéro. |
| **HT3 → H3, H5** | Delta matériel sans forçage admis. | Chaque clé interdite présente avec `"1"`, `"0"` ou `""` : refus avant factory. Ne pas traiter `"0"` comme absence. |
| **HT4 → H1, H4** | Matériel seulement ambiant, delta vide : six contextes ambiants retirés, forçages logiciels hérités. CPU/sans déclaration et logiciel explicite conservent leur politique. | Une déclaration non exactement matérielle ne déclenche pas ce nettoyage. Aucun nouveau rejet sémantique général imposé au runner. |
| **HT5 → H4, H5, I1, I3** | Même runner : CPU→matériel→logiciel, puis ordre inverse ; environnement effectif propre à chaque appel. | Insérer un refus HT2/HT3 entre deux appels : parent identique et appel suivant non contaminé. |
| **HT6 → H5, H6** | Doubles succès puis nonzero avec sorties synthétiques ; capture et outcome existants conservés. | Refus de transport propagé comme panne pipeline, sans faux code enfant favorable ni précondition GPU. Réutiliser les contrôles existants de causes/capture ; vérifier le chemin de propagation effectivement retenu. |

Pour HT2/HT3, ne pas figer ici une nouvelle classe d’exception ou un code public absent du contrat : l’auteur réutilise la convention du pipeline et documente l’attendu causal avant RED.

### 5. Registre bidirectionnel

| Obligation | Localisation et raison | Tests |
|---|---|---|
| H1–H3 | `runner.py:SubprocessRunner.run`, construction de l’environnement avant factory | HT1–HT3 |
| H4–H5 | Même symbole, copie par invocation et nettoyage des contextes | HT4–HT5 |
| H5–H6 | Propagation du refus et capture existante ; aucun nouveau schéma | HT6 |

**Sens inverse :** HT1–HT6 correspondent aux obligations ci-dessus ; aucun test de performance, driver réel ou scénario GPU ajouté. Le test de transport existant contribue à H4/H5 mais ne couvre pas H2/H3. Sa présence est **static-presence**, pas une exécution de ce delta.

**État :** H1–H6 = **GAP** jusqu’aux preuves qualifiées ; aucun COVERED revendiqué.

### 6. Budget additionnel

Base relative **100** :

- Setup : **25** — fixtures parent/delta et faux factory strict.
- Planifié : **56,25** — HT1/HT3 : 18 ; HT2 : 12 ; HT4/HT5 : 16 ; HT6 et revue : 10,25.
- Buffer investigation/panne/ré-audit : **18,75**.

Après setup : **75 % planifiés / 25 % buffer**. Priorité aux refus et à l’environnement effectif ; pas de campagne globale ni infrastructure supplémentaire.

### 7. Sortie et preuves

1. Addendum committé avant auteur unique runner/tests.
2. Tests RED présentés avant implémentation ; RED causal observé par le lead, puis GREEN et non-régression ciblée.
3. Faux `popen_factory` strict uniquement : **aucun processus enfant réel**, moteur, GL ou outil de build.
4. Revue indépendante ciblée du déclencheur, de l’ordre nettoyage/application et des refus ; corrections/ré-audit avant intégration, puis revalidation Linux synthétique.
5. Pour chaque preuve : HEAD/delta, SHA256 pertinents, source/date, commande, runner, environnement, attendu/observé et comptes découverts/exécutés/échoués/skipped. Changement de candidat ⇒ preuve du delta à requalifier ; zéro test ou skip requis ≠ PASS.

**Limite hors correction :** autres mécanismes externes de sélection du driver non couverts. La présence d’un environnement assaini ne prouve pas un renderer matériel ; contrôle GL réel et cible exacte restent **NON_EXECUTE**, à qualifier ultérieurement. Aucun nouveau GO utilisateur nécessaire pour cette proposition bornée.

## assistant — 2026-10-30T07:41:16.998Z — msg_18bc181410016mxX1ZrmReZlE0

# Complément QA proposé — helper Windows et raccordement CMake

À ajouter à `.opencode/evidence/mission-uniform-pipeline/qa-plan.md`, **sans remplacer les §11–12 acquis**. Persistance et commit par le lead avant les worktrees auteurs.

## 1. Candidat, portée et preuves

**HEAD fourni par le lead :** `a7aacb9bb10346c28a637d8264aa539a12f3d733`. Intégrations déclarées : runner `358921c`, Linux `7a1acbf`, adaptateur Windows `86b51ad`. Pas de vérification Git effectuée ici.

Lectures locales réalisées : helper Windows, tests/fixture dispatch, oracle de fermeture, CMake parents et workloads, contrat Linux. Le contrat Windows et le plan existant lus précédemment restent applicables.

**Constats source :**
- Le helper reste fermé avant dispatch pour toute `VerificationPhase`.
- Son dispatch contient encore un chemin `run`.
- `$root` et le tooling legacy sont définis **après** ce dispatch : les fixtures qui injectent `$root` ne prouvent donc pas le binding réel.
- Le CMake racine ne déclare pas encore `ARTIST_BUILD_VERIFICATION` ; `ARTIST/CMakeLists.txt` n’inclut pas `verification`.
- `ARTIST/verification/CMakeLists.txt` déclare déjà les deux workloads, sans découverte ni commande de lancement.

Deux branches indépendantes :
- **A :** helper Windows fonctionnel, **prepare/build uniquement**.
- **B :** raccordement CMake optionnel. Proposition minimale : **pas de nouveau workflow dans cette tranche**, afin d’éviter un nouveau contrat de campagne ; conserver et contrôler les workflows existants si touchés. Toute documentation prétendant utiliser le helper fonctionnel attend son intégration.

Aucun fichier modifié, aucune commande exécutée. Les nouvelles obligations restent **GAP**, pas COVERED.

## 2. Branche A — obligations et risques

### Obligations explicites

- **A1 :** seules prepare/build sont accessibles ; run, flags incompatibles, payload invalide et prepare sans opt-in refusés avant bootstrap, environnement MSVC ou outils.
- **A2 :** payload privé à quatre champs validé et lié au bundle admis ; aucune recréation/réparation par le helper.
- **A3 :** prepare décrit/exécute uniquement Conan 2 puis configure CMake ; configuration, arbre, targets et `PRE_TEST` cohérents. Aucun build/test/workload implicite.
- **A4 :** build ne fait que construire les cibles sélectionnées dans l’arbre préparé, sans bootstrap, Conan, configure ou réparation.
- **A5 :** environnement MSVC visible aux seules commandes possédées ; état du processus appelant restauré après succès, nonzero et exception.
- **A6 :** legacy préservé ; run commun direct et capture core primaire conservés. Aucun helper-run commun, nouveau schéma ou API publique.
- **A7 :** région native pinée inchangée ; aucun repin automatique. Tests limités aux fragments source relus et effets factices.

**Contrôles inférés :** sentinelle anti-fallthrough legacy, compteur ordonné d’effets, mutations ciblées du fragment, comparaison de l’environnement avant/après. Ce sont des moyens de preuve, pas des fonctionnalités supplémentaires.

### Top trois risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| Ouverture permettant bootstrap/run ou effets avant admission | 5 | 4 | **20** |
| Fixture masquant un binding réel incomplet ou mauvais arbre/configuration | 4 | 4 | **16** |
| Environnement/codes/bruts perdus après échec | 4 | 3 | **12** |

- **RA1 :** déclencheur : retrait de la garde globale sans remplacement discriminant, flags ou phase run acceptés. Portée : outils réels et phases interdites. Détection actuelle : oracle global de fermeture. Mitigation actuelle : helper fermé. **Gap :** admission fine avant tout effet et refus run.
- **RA2 :** déclencheur : `$root`/tooling injectés seulement dans le test, payload remplacé par variables fixture, configuration divergente. Portée : prepare/build et attribution des sorties. Détection actuelle : tests dispatch avec variables injectées. Mitigation : validation wrapper existante. **Gap :** chaîne source réelle garde→binding→dispatch, sans initialisation cachée.
- **RA3 :** déclencheur : Conan/CMake nonzero, exception MSVC ou dispatch, restauration incomplète. Portée : commandes suivantes, diagnostic et preuves. Détection actuelle : tests de restauration/codes présents. Mitigation : `finally` du dispatch. **Gap :** composition du futur binding et conservation de la première cause.

## 3. Branche A — scénarios et remplacement de l’ancien oracle

### Mapping obligatoire avant modification

`test_wrappers.WindowsRealHelperBarrierTests.test_real_helper_remains_closed_before_dispatch` prouve actuellement **la fermeture globale**, pas le fonctionnement prepare/build.

Avant de le modifier, inscrire explicitement :

| Ancienne propriété | Remplacement attendu |
|---|---|
| Aucun dispatch réel accessible | Admission prepare/build seulement après validation |
| Aucun effet avant refus | Tests A-T1 avec journal d’effets vide |
| Pas de run/helper fallback | Tests A-T1 et sentinelle legacy |
| Helper complet non exécuté par les tests | Revue du lanceur et extraction bornée A-T0 |

L’ancien oracle devient périmé **seulement après** adoption de ce mapping. Ne pas simplement supprimer son assertion pour obtenir GREEN.

| Groupe → obligations | Nominal | Négatif et oracle |
|---|---|---|
| **A-T0 → A7** | Fragments réels garde/binding/dispatch, marqueurs uniques, syntaxe valide ; feuilles d’effets factices explicitement admises. | Extrait inattendu, appel non autorisé ou pin changé : HARNESS/INCONCLUSIVE, jamais RED comportemental. Aucun fallback helper complet. |
| **A-T1 → A1, A6** | Prepare autorisé et build valide atteignent chacun leur dispatch. | Opt-in absent, flags incompatibles, phase run/unsupported, payload absent/invalide : refus causal, zéro effet bootstrap/env/outils, pas de fallthrough legacy. |
| **A-T2 → A2, A3** | Payload réel synthétique quatre champs ; Debug/Release, chemins avec espaces, targets workloads seuls puis sélection incluant tests. | Mauvais bundle/arbre/configuration, champ supplémentaire ou commande run transportée : refus avant effet ; source payload intacte. |
| **A-T3 → A3** | Journal exact Conan→CMake ; profils host/build et configuration concordants ; vérification activée ; tests sélectionnés ⇒ `BUILD_TESTING=ON` et `PRE_TEST`, sinon OFF. | Conan nonzero : pas de CMake. Aucun événement build, CTest, run, installation implicite hors prepare autorisé. |
| **A-T4 → A4** | Une commande CMake build, arbre/cibles/jobs exacts. | Arbre non admis ou commande nonzero : pas de bootstrap/configure/réparation ; code causal conservé. |
| **A-T5 → A5, A6** | Environnement MSVC présent au fake child puis restauration exacte. | Succès/nonzero/exception, clés initialement absentes ou vides : état final identique ; premier code et bruts disponibles conservés. |
| **A-T6 → A6, A7** | Revue du chemin legacy et non-régression ciblée existante. | Pas de chute de verification vers legacy, pas de helper-run ajouté, région native inchangée. |

**RED recevable :** rejet « non implémenté » sur un cas prepare autorisé dont le harness est valide. Fonction absente, erreur de parsing, PowerShell indisponible ou fixture cassée ≠ RED causal.

Le lead relit avant lancement le programme PowerShell **test-owned**, son extraction, ses feuilles factices, sa frontière de spawn et ses sorties. Le helper complet et toute nouvelle feuille native réelle restent interdits à l’exécution dans cette mission. Pas de discovery Windows globale.

## 4. Branche B — obligations, risques et scénarios

### Obligations explicites

- **B1 :** option racine `ARTIST_BUILD_VERIFICATION`, défaut **OFF**.
- **B2 :** `add_subdirectory(verification)` conditionnel dans `ARTIST/CMakeLists.txt`, sans conditionnement accidentel à `BUILD_TESTING`.
- **B3 :** réutiliser les cibles intégrées ; pas de custom command, découverte ou lancement moteur ajouté au configure/build.
- **B4 :** préserver Conan 2 avant configure, Ninja, configurations et découverte `PRE_TEST` existantes ; aucune exécution CMake réelle pour les tests de cette tranche.
- **B5 :** si workflow existant touché : activation manuelle, profils explicites, pas de fallback logiciel/cloud ; Linux DISPLAY et Windows MSVC restent préconditions opérateur.

**Inférences :** vérifications structurelles contextualisées, commentaires/chaînes ignorés, contre-exemples synthétiques pour s’assurer que les contrôles ne sont pas de simples recherches de mots.

### Top trois risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| Workloads activés ou exécutés implicitement | 5 | 3 | **15** |
| Option ON sans raccordement réel ou dépendant de BUILD_TESTING | 4 | 3 | **12** |
| Workflow déclenchant campagne/fallback non demandé | 5 | 2 | **10** |

- **RB1 :** déclencheur : défaut ON, inclusion inconditionnelle ou commande de lancement. Portée : builds ordinaires et suites usuelles. Détection actuelle : leaf sans lancement, parents non raccordés. Mitigation : option dédiée prévue. **Gap :** garde OFF et absence de nouvelle exécution à contrôler.
- **RB2 :** déclencheur : mauvais chemin/scope ou inclusion dans `if(BUILD_TESTING)`. Portée : livraison sélectionnable des deux workloads. Détection actuelle : lecture CMake. Mitigation : leaf existant. **Gap :** raccordement parents et matrice logique option/tests.
- **RB3 :** déclencheur : événement automatique, profil implicite ou machine cloud ajoutée. Portée : campagnes non autorisées. Détection actuelle : revue des workflows concernés seulement. Mitigation proposée : aucun nouveau workflow dans cette tranche. **Gap :** vérifier le diff final ; risque retiré du delta si aucun workflow changé, pas déclaré couvert par un test GPU.

**Scénarios :**
- **B-T1 → B1/B2 :** analyser source et matrice logique OFF/ON × BUILD_TESTING OFF/ON ; OFF exclut le sous-répertoire, ON le raccorde indépendamment des tests.
- **B-T2 → B2/B3 :** deux noms de cibles existants atteignables statiquement ; négatifs synthétiques mauvais chemin, inclusion inconditionnelle, scope tests seul et lancement ajouté : contrôles doivent discriminer.
- **B-T3 → B3/B4 :** audit structurel du diff : aucun configure/build/CTest moteur exécuté, aucun changement de générateur/toolchain/découverte opportuniste.
- **B-T4 → B5, conditionnel :** si YAML touché, parser statique disponible et revue des triggers/jobs/profils ; contre-exemples trigger automatique/fallback. Sinon **hors delta**, pas nouveau test artificiel.

Ces preuves sont statiques/synthétiques : elles ne démontrent ni configure CMake réel ni compilation des workloads.

## 5. Registre et réemploi

- `test_wrappers.py:WindowsRealHelperBarrierTests…` → ancienne fermeture ; mapping A-T0/A-T1 avant remplacement.
- `test_dispatch.py:WindowsDispatchTests` et `dispatch_fixture.ps1` → ordre, argv, codes, restauration ; étendre seulement pour la chaîne réelle manquante.
- `build-windows.ps1:Resolve-VerificationInputs` et dispatch → A2–A5.
- Tests adaptateur déjà intégrés, notamment payload ownership/build reuse/common capture → A2/A6 ; réutiliser leurs preuves valides, sans duplication.
- `CMakeLists.txt`, `ARTIST/CMakeLists.txt`, `ARTIST/verification/CMakeLists.txt` → B-T1–B-T3.
- Chaque nouveau test doit pointer une obligation ci-dessus. Aucun orphelin établi ; tests privés run historiques restent hors preuve du run commun.

**Gap ciblé à transmettre au QA generator ultérieur :** composition réelle synthétique **WindowsAdapter→SubprocessRunner→faux popen_factory**, vérifiant l’environnement effectivement reçu. Ne pas dupliquer ce travail dans A.

**Limite connue :** `Dispose` avec writers hérités du chemin privé run ne constitue pas une preuve du run commun ; hors périmètre de correction ici.

## 6. Budget et sortie

Budget relatif additionnel **100**, sans promesse horaire :
- Setup **25** : revue/extraction fixtures, mapping ancien oracle, données synthétiques.
- Planifié **56,25** : A admission/binding 20 ; A phases/env/causes 18 ; B structure 10 ; revue/non-régression 8,25.
- Buffer **18,75** : investigation, panne harness, ré-audit.

Soit **75/25 après setup**. Couper extensions/workflow neuf avant les contrôles ≥10 ; ne pas consommer le buffer pour conserver de la cosmétique.

**Sortie :**
1. Plan committé avant auteurs ; ownership A/B distinct et sans fichier partagé.
2. RED causal puis GREEN/non-régression ciblés par le lead, uniquement sur fixtures relues.
3. Revue indépendante ciblée garde/binding/effets pour A, garde/raccordement CMake pour B ; corrections et ré-audit.
4. Preuves enregistrées avec HEAD, delta, SHA256, source/date, commande, runner/environnement, attendu/observé, découverts/exécutés/échoués/skipped. Candidat différent ⇒ requalification du delta ; zéro test/skip requis ≠ PASS.
5. Intégration effective avant documentation finale et consommateurs.

Qualification native Windows/Linux, configure/build C++, CTest réel, GPU et mesures restent **NON_EXECUTE**. Livraison pour qualification, aucun gain requis. Aucun nouvel accord utilisateur, droit ou périmètre d’exécution n’est créé par ce plan.

## assistant — 2026-10-30T07:46:30.671Z — msg_18bc64a88001mKS6VfHDOI2Ka6

## Rectification B — workflow manuel obligatoire

L’omission proposée du workflow est retirée : `.github/workflows/verification-pipeline.yml` est un livrable explicite du contrat, **pas un GAP-defer accepté**. Les autres sections A/B restent inchangées.

### B5 — Déclenchement et machines

**Exigences explicites :**
- `workflow_dispatch` uniquement ; aucun push, PR, schedule, workflow_call ou déclenchement automatique.
- Machines opérateur préparées, self-hosted Windows/Linux ; aucune réservation hosted/cloud, installation ou réparation implicite.
- Platform/profile/phase sélectionnés explicitement dans des ensembles fermés.
- Hardware : vendor/renderer exacts, non vides ; un label de runner/GPU ne qualifie pas le matériel. Linux DISPLAY et Windows MSVC restent des préconditions machine.
- Aucune invocation du workflow pendant cette mission. Livraison **pour qualification**, commandes natives **NON_EXECUTE**.

**Risque : I5 × P3 = 15.**  
Déclencheur : trigger automatique, runner hosted ou fallback logiciel. Portée : lancement non demandé et attribution matérielle erronée. Détection actuelle : contrat et revue à effectuer, aucune exécution probante. Mitigation : déclencheur manuel et machines explicitement préparées. **Gap :** contrôle structurel du workflow final et de ses chemins de refus.

### B6 — Entrées, phases et preuves

**Exigences explicites :**
- Valider les entrées avant toute commande de phase ; conserver les chaînes vendor/renderer sans normalisation destructive.
- Transport via environnement/arguments correctement délimités, sans interpolation libre des expressions d’inputs dans le script shell.
- List/plan consultatifs sans bootstrap, construction ou exécution implicite.
- Chaque phase reste explicitement choisie ; prédécesseur absent ⇒ refus, jamais cascade prepare/build/run réparatrice.
- Conserver identités, journaux, résultats bruts et codes. Artefact absent, upload réussi ou enfant code zéro ne suffisent pas à produire PASS.
- Checkout et actions d’artefacts à versions explicitement pinées selon les pratiques locales ; aucun nouveau budget automatique.

**Risque : I5 × P3 = 15.**  
Déclencheur : input injecté/non validé, cascade de phases ou absence de preuve masquée. Portée : commandes sur machine opérateur et conclusions du pipeline. Détection actuelle : séparation CLI existante ; raccordement workflow non qualifié. Mitigation : choix fermés, transport littéral et refus des prédécesseurs manquants. **Gap :** vérifier la composition workflow→argv et la conservation après échec.

### T4 — Oracles ciblés, statiques et synthétiques

| Contrôle | Nominal | Contre-exemple discriminant |
|---|---|---|
| **T4a → B5** | Seul `workflow_dispatch`, branches Windows/Linux self-hosted bornées. | Ajouter trigger automatique, `ubuntu-latest`/hosted ou sélection libre de runner : rejet par contrôle et revue. |
| **T4b → B5/B6** | Choix admis transmis littéralement ; hardware avec deux identités exactes. | Choix inconnu, identité vide, input hostile quotes/retours ligne/métacaractères : refus ou préservation littérale, jamais commande supplémentaire. |
| **T4c → B6** | List/plan n’appellent que leur mode ; chaque phase produit uniquement ses argv attendus sous doubles. | Prédécesseur absent, branche install/bootstrap ou cascade ajoutée : refus sans commande de remplacement. |
| **T4d → B6** | Codes et artefacts disponibles conservés après succès comme échec. | Résultat absent, logs partiels ou échec enfant suivi d’upload réussi : aucune conclusion favorable fabriquée. |
| **T4e → B5/B6** | Actions pinées et préconditions opérateur documentées. | Label GPU traité comme preuve, fallback software, installation ou cloud ajouté : finding bloquant. |

Utiliser un parseur YAML déjà disponible, sinon un contrôle textuel structuré borné complété par revue ; **aucune installation**. Les doubles vérifient les argv sans lancer shell, outils natifs ou workflow. Un contrôle textuel ne prouve pas l’exécution GitHub Actions.

**Registre :** workflow → B5/B6 → T4a–T4e ; documentation des préconditions → B5/T4e. Chaque test ajouté doit conserver cette attribution. Statut actuel : **GAP**.

**Sortie :** choix détaillés auteur présentés avant implémentation ; complément intégré au plan et committé avant tout auteur ; contrôles synthétiques attribués et revue indépendante ciblée avant intégration. Aucun nouveau budget, framework, lancement ou droit proposé.

## assistant — 2026-10-30T08:43:13.001Z — msg_18bfa34e5001YZTiHz7EWkovDY

# Addendum QA — preuves privées du helper Windows

**Destination :** compléter le même `qa-plan.md`, sans nouveau dossier ni réouverture des plans acquis. Le lead persiste/committe avant nouveaux tests/source et avant worktrees auteurs.

## 1. Référence et état observé

Contrat lu : `windows-helper-evidence-v1.md`, commit **fourni par le lead** `b8d036a72d9ebfb3f972023c12c7c36e1a1e3097`.

Code observé :
- `BundleSink._prepare_windows_helper` matérialise et empreinte le payload.
- `_admit_windows_helper` réadmet actuellement ce payload.
- `prepare`/`build` traitent succès, erreur et interruption, mais aucune collecte de la nouvelle population privée n’est présente dans les portions examinées.
- `BundleSink.command` conserve la capture commune ; elle reste primaire.

Les **7 PASS + RED1 NOTIMPLEMENTED** et les **18 tests haut niveau** sont des preuves transmises par le lead, non réexécutées ici. Ils ne démontrent pas le nouveau wiring des feuilles ni le collecteur.

Aucun edit, test, processus ou appel natif réalisé ici. Aucun refus de lecture rencontré.

## 2. Obligations du delta

### Explicites

- **W1 — Contexte privé :** non réentrant, propre à l’invocation, créé après admission, retiré en `finally`. Environnement enfant sous forme de map ; aucune mutation de l’environnement OS, aucun cache/global/fallback.
- **W2 — Processus et budget :** un Process par commande ; factory propriétaire du Dispose, primitives propriétaires des streams. Échéance globale restante ; absence de code, timeout, exception ou cleanup défaillant jamais favorables.
- **W3 — Journaux fermés :** rôles, noms et champs **exactement ceux du contrat §25–49** ; création exclusive ; ligne de commande réellement transmise au StartInfo ; bruts partiels conservés.
- **W4 — Témoin :** `toolchain.json` fermé après prepare réussi, exactement les douze chaînes du contrat. Son SHA256 identifie les octets déclaratifs, **pas les binaires outils**.
- **W5 — Build sans réparation :** témoin empreinté dans PreparationReceipt, réadmis avant adaptateur/tooling ; cohérence configuration/arbre et préconditions CMake/Ninja bornées ; aucun bootstrap/configure implicite.
- **W6 — Collecte core :** seulement pour transport Windows possédé/admis, sous lease, également après nonzero/interruption/exception, avant receipt. Succès conditionné au témoin et aux rôles obligatoires.
- **W7 — Ownership et causes :** fichiers réguliers, absence de liens/reparse/alias/hardlinks interdits, no-clobber ; population inconnue signalée sans adoption/suppression. Première cause et priorité existante conservées.
- **W8 — Compatibilité :** aucun schéma public, capability parser ou deuxième capture workload ; wrappers/primitives natifs pinés inchangés.

### Contrôles inférés

Sentinelles d’effets, compteur de factory, horloge factice, comparaison complète du parent et mutants synthétiques ciblés. Ils servent les obligations sans créer de nouvelles fonctionnalités.

## 3. Top trois risques

| Risque | I | P | Priorité |
|---|---:|---:|---:|
| Receipt favorable sans preuve privée valide/réadmise | 5 | 4 | **20** |
| Collecte adoptant un fichier étranger ou perdant les partiels | 5 | 3 | **15** |
| Wiring enfant/budget/cleanup masqué par les doubles haut niveau | 4 | 4 | **16** |

**R1 :** déclencheur : code zéro sans témoin/rôle obligatoire, témoin modifié ou étranger. Portée : préparation et construction attribuées à tort. Détection actuelle : admission du payload et empreintes génériques. Mitigation : receipts liés au candidat. **Gap :** validation privée, collecte obligatoire et réadmission du témoin.

**R2 :** déclencheur : alias, répertoire ancien, fichier inconnu, exception pendant collecte. Portée : intégrité des artefacts et diagnostic. Détection actuelle : confinement et créations exclusives existants. Mitigation : chemins bornés et lease. **Gap :** ownership de toute la population, conservation partielle et causes secondaires.

**R3 :** déclencheur : test remplaçant la feuille child, timeout renouvelé par commande, map persistante ou Dispose en erreur. Portée : commandes helper et conclusions de succès. Détection actuelle : tests haut niveau et primitives pinées. Mitigation : contrat factory/contexte privé. **Gap :** tests aux nouvelles coutures avec factory mémoire et exporter réel borné.

Réévaluer après intégration et première surprise ; ne pas changer les scores PM.

## 4. Branche core — tests sans processus

Fake runner créant exclusivement des données synthétiques dans les destinations possédées. Aucun appel Git, outil ou processus réel.

| Groupe → obligations | Nominal | Négatif / oracle discriminant |
|---|---|---|
| **C1 → W3/W4/W6** | Prepare puis build factices publient population conforme ; empreintes présentes avant receipt. | Enfant zéro mais témoin ou rôle obligatoire absent : receipt non favorable, cause explicite. |
| **C2 → W4/W5** | Témoin valide lié à configuration/arbre ; build réadmet ses octets. | Absence, mutation, témoin étranger, clé inconnue/manquante, doublon JSON, mauvais type/format : refus avant appel adaptateur/runner, aucun rewrite. |
| **C3 → W3/W6/W7** | Collecte disponible après succès, nonzero, exception et interruption ; capture commune conservée. | Bruts partiels ou trace incomplète : octets disponibles conservés, anomalie visible, aucun témoin inventé ni succès. |
| **C4 → W3/W7** | Population fermée conforme, transports temporaires autorisés restants collectés en échec. | Rôle/fichier/répertoire inconnu, collision ou ancien dossier : non favorable, inconnu laissé sur disque sans adoption ; aucun parcours récursif arbitraire. |
| **C5 → W7** | Fichiers réguliers distincts admis. | Symlink/reparse/hardlink/alias vers autre artefact : refus sans clobber. Précondition filesystem non disponible ⇒ limite explicite, pas PASS. |
| **C6 → W6/W7/W8** | Double générique sans transport helper inchangé ; effets sous lease. | Échec helper suivi d’anomalie de collecte : première cause conservée, secondaire enregistrée ; code selon priorité existante, jamais zéro fabriqué. |

### Mapping préalable des fakes historiques

Avant leur modification, fournir une table **fichier:symbole → obligation métier existante → données privées manquantes → fixture synthétique ajoutée**.

Pour les succès Windows activant réellement le transport helper :
- produire les rôles obligatoires, traces/bruts et témoin minimal conformes ;
- conserver les assertions sur payload, ordre des phases, codes et capture ;
- ne pas contourner la validation ni accepter un témoin absent.

Ne pas ajouter cette population aux doubles génériques non-helper. Les anciens succès sans witness deviennent des fixtures à compléter, pas une raison d’affaiblir le contrat.

## 5. Branche PowerShell — feuilles et exporter

Nouvelle fixture **test-owned**, data/factory mémoire, extraction bornée relue et sentinelles sans fallback. Aucune exécution du helper complet ou d’une feuille native réelle.

| Groupe → obligations | Nominal | Négatif / oracle discriminant |
|---|---|---|
| **P0 → W8** | Syntaxe/extraction/pins et factory factice validés avant RED. | Fonction absente, harness cassé ou pin inattendu : HARNESS/INCONCLUSIVE, pas RED causal. |
| **P1 → W1** | Map enfant copiée ; null retire, vide reste présent ; root issu du vrai binding source. | Contexte absent/réentrant refusé ; deux invocations sans contamination ; environnement OS parent identique après succès/échec. |
| **P2 → W2/W3** | Factory observe programme/cwd absolus, command line exacte, environnement et Process unique. | Code absent, nonzero, exception : pas de succès ; Dispose attendu, cause et sorties disponibles conservées. |
| **P3 → W2** | Horloge factice : chaque lancement reçoit le budget restant. | Budget épuisé avant commande suivante : zéro lancement supplémentaire ; timeout non réinitialisé ; cleanup défaillant visible. |
| **P4 → W3** | Exporter source réel borné écrit journaux fermés et transports allowlistés, exclusivement. | Collision, nom inattendu, cleanup transport défaillant : pas d’écrasement ni suppression étrangère ; traces/partiels conservés. |
| **P5 → W4/W5** | Prepare publie le témoin uniquement après réussite ; build lit les mêmes déclarations sans les modifier. | VS explicite invalide, témoin divergent, cache/toolchain/build.ninja absent ou incohérent : refus avant tooling/env/process, aucun fallback ou réparation. |

Le nouveau wiring/exporteur ne doit pas être remplacé intégralement par les doubles : placer les doubles aux feuilles d’effets prévues. Relecture lead obligatoire avant mise à jour d’un pin de source de fixture ; **WRAPPERS/NATIVE restent intangibles**.

Le gap connu Dispose/writers hérités n’est ni rouvert ni déclaré résolu.

## 6. Registre et couture

| Obligations | Localisation / preuve visée |
|---|---|
| W1/W2 | Helper : contexte privé, factory et binding → P1–P3 |
| W3/W4 | Exporter et témoin privés → P4/P5 ; collecteur core → C1–C4 |
| W5 | `evidence.py:_admit_windows_helper`, `build`, helper admission build → C2/P5 |
| W6/W7 | `evidence.py:prepare`, `build`, collecte avant `_finish` → C1/C3–C6 |
| W8 | Diff, pins et tests génériques conservés → P0/C6 |

**Sens inverse :** chaque C/P doit pointer ces obligations. Les 18 tests haut niveau restent utiles pour leurs assertions existantes mais ne remplacent pas P1–P5. Aucun orphelin établi par cette inspection bornée.

**Après les deux intégrations seulement :** couture synthétique démontrant que le témoin produit selon le contrat est réellement empreinté dans `PreparationReceipt.artifacts`, puis réadmis avant build ; mutation ⇒ refus avant adaptateur/tooling. Ce test ne vaut pas clôture automatique du candidat.

## 7. Budget, preuves et sortie

Base relative additionnelle **100** :
- Setup **25** : mappings historiques, données fermées, factory/horloge/exporter fixture.
- Planifié **56,25** : core 24 ; PowerShell 24 ; couture/revue 8,25.
- Buffer **18,75** : investigation, panne harness, ré-audit.

Soit **75 % planifiés / 25 % buffer après setup**. Prioriser les refus et risques ≥10 ; aucun nouvel outil/framework/campagne.

**Sortie :**
1. Addendum et mappings committés avant auteurs ; ownership core/helper distinct.
2. Baseline harness valide, RED causal avant implémentation, GREEN minimal et non-régression ciblée par runner lead.
3. Collecte sur tous chemins, no-clobber, causes conservées et réadmission démontrées.
4. Revue indépendante ciblée, corrections/ré-audit, puis couture après intégration.
5. Chaque preuve conserve HEAD/delta, SHA256, source/date, commande, runner/environnement, attendu/observé et comptes découverts/exécutés/échoués/skipped. Zéro test/skip requis ≠ PASS ; candidat différent ⇒ requalification.

**Statuts actuels :** W1–W8 **GAP** pour ce delta ; source observée = `static-presence`, résultats historiques = exécutions attribuées au lead. Qualification native et identité binaire des outils **non démontrées**. Aucun workflow/CMake, moteur, GPU, installation ou cloud à exécuter dans cette tranche.