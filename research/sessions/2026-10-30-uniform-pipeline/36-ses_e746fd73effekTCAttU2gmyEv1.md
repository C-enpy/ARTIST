# pool-25 (@correctness-reviewer teammate)

Session: ses_e746fd73effekTCAttU2gmyEv1
Parent: ses_e76ba2716ffdW1hBGnJ0AgYfKC

Public messages only; not an execution proof. Tool payloads, reasoning and worker briefs omitted.

## assistant — 2026-10-30T06:47:37.362Z — msg_18b90608e001XJL0qAD1ZAWg6G

Je vais comparer les fichiers et les preuves existantes au contrat ratifié, puis suivre leurs consommateurs. Je ne lancerai aucun processus ni test.

## assistant — 2026-10-30T06:52:33.651Z — msg_18b94e5f0001kEJAT7v9hRckoo

## Avis : correction requise avant intégration

Candidat relu : `3e697590c03869f53b64aec8dde0cd706bceff52`.

### F1 — P2, bloquant contractuel : forçage logiciel hérité en profil hardware

**Localisation :** `scripts/verification/adapters/linux.py:116–126`, `LinuxAdapter._environment` ; consommateur `scripts/verification/runner.py:44–47`, `SubprocessRunner.run`.

**Scénario :** l’opérateur lance une sélection `opengl-hardware` depuis un environnement contenant `LIBGL_ALWAYS_SOFTWARE=true` et `GALLIUM_DRIVER=llvmpipe`.

**Attendu :** aucune transmission du forçage logiciel au processus hardware, ou refus explicite avant son lancement. Cette exigence figure aussi dans `LINUX_BINDING_RED_READY.md:100–103`.

**Observé, démontré statiquement :** l’adaptateur fournit les six variables d’attribution, mais ne neutralise pas ces deux variables. Le runner copie l’environnement parent, retire uniquement les variables d’attribution, puis applique `env_delta`. Les enfants hardware héritent donc du forçage logiciel.

**Impact :** une qualification hardware peut lancer un contexte logiciel puis échouer malgré un matériel disponible. Les gardes de cible exacte empêchent normalement un faux PASS : ce finding ne démontre pas leur contournement.

**Vérification minimale proposée, non exécutée :** injecter les deux variables dans un environnement synthétique, décrire une invocation hardware et utiliser un faux `popen_factory`. Vérifier que l’environnement transmis ne force plus le logiciel, ou qu’un refus précède l’appel. Le test actuel `test_binding_adapter.py:140–146` vérifie seulement `env_delta`, pas l’environnement effectif.

### Contrôles et preuves relus

- Séparation prepare/build/run, mapping des exécutables admis, chemins lexicaux, identités répétables et sorties distinctes.
- Sélection CTest ancrée avec groupes ordinaires CMSys, même arbre/configuration.
- Helper build-only limité aux cibles explicites ; branches historiques et gardes Release/ASan conservées.
- Manifeste et journaux GREEN : **34 + 6 + 9 + 10 tests**, tous annoncés et journalisés OK par le lead.

### Limites

Aucune édition, exécution ou relance effectuée. Admission des identités, confinement réel et leases restent des garanties du core, pas des descriptions Linux. Serena est rattaché au canonique ; le candidat a été examiné directement dans son worktree.

Linux natif, toolchain, moteur, GPU et performances restent **NON_EXECUTE**. Aucun autre défaut bloquant identifié dans le périmètre relu ; ce constat n’est pas une preuve exhaustive.

## assistant — 2026-10-30T07:30:34.104Z — msg_18bb7b1f4001EWc27sIhoMAF2X

## Avis borné : F1 levé statiquement dans la composition corrigée

**Composition examinée :**
- production Linux inchangée : `3e697590c03869f53b64aec8dde0cd706bceff52` ;
- correctif commun intégré : `358921c42b83f5ae619c97fb8f7c1a66700ef4e6` ;
- tests consommateur et note : `fa78654d4d62e8a0b31ef2b2c6cc4be183db3505`.

### Localisation / impact

Dans `scripts/verification/runner.py:47–57`, `SubprocessRunner.run` :
- exige les six champs explicites sous déclaration hardware ;
- refuse toute réintroduction des deux variables logicielles dans le delta ;
- retire ces variables de la **copie enfant**, avant application du delta.

Cela ferme précisément F1 pour les workloads, découvertes et exécutions CTest décrits par Linux. Le parent reste intact ; CPU et logiciel conservent leur politique. **Ce correctif appartient au core, pas au candidat Linux.**

### Test discriminant

`test_effective_linux_environment.py:170–189` examine bien l’environnement effectif reçu par la factory : trois combinaisons de forçage × trois catégories d’invocation. Les contrôles du parent, des six champs, de DISPLAY et des captures possédées complètent cet oracle, sans processus réel.

Le manifeste et le journal RED confirment **4 tests, 9 échecs hardware causaux**, baselines CPU/logiciel favorables. Les empreintes Linux consignées restent celles du candidat initial ; le complément remis est limité au test et à sa note.

### Réception

**Avis favorable à l’intégration sur le core corrigé**, sans autre changement Linux pour F1. La clôture reste conditionnée au **GREEN consommateur après intégration**, pas encore exécuté. Aucun GREEN n’est exigé sur le worktree conservant volontairement l’ancien runner.

Aucune exécution ou édition par moi. Aucun nouveau finding dans ce réaudit ciblé ; pas de preuve exhaustive. Linux natif et GPU restent **NON_EXECUTE**.