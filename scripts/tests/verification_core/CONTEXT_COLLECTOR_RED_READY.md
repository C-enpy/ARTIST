# RED_READY — collecteur des contextes historiques

## Candidat et portée

Base reçue : `3bd72fc` ; fondations core intégrées `107a2d3`, parser et producteur
historiques présents sur disque. Rapports59/61/69/73, contrat principal,
`legacy-context-observation-v1.md` et QA §10 lus avant édition. Cette remise ne
réimplémente pas les fondations de la tâche initiale et ne les dit pas terminées.

Worktree natif :
`C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-cwsmsw-pool-14`.
Branche : `ensemble-3b74f9ed-91ef-410f-8ac6-431ecb896a6d`.

Fichiers de la tranche :

- `test_context_collector_contract.py` : 20 méthodes, plus variations subTest ;
- `CONTEXT_CONSUMER_PLAN.md` : actualisation du plan conservé ;
- ce document de remise.

Aucune source de production modifiée. Aucun test existant ou oracle affaibli.
Pas de modification de runner, schéma, adaptateur, fixture native, CMake ou scène.

## Plan et preuves attendues

1. Le lead relit le diff, puis exécute le RED ciblé ci-dessous dans ce worktree.
2. Nominal : deux cas OpenGL et les cas unitaires de la sélection lifetime réelle
   du catalogue, données synthétiques uniquement. Le contrôle causal attendu est
   `exit_code=3` au lieu de `0` à cause de la garde hardware CTest encore active,
   pas un ImportError. La garde CLI et la garde d'exécution sont toutes deux
   exercées. Les tests vérifient qu'un vrai faux-producteur a été atteint : le
   code3 préventif ne peut pas satisfaire à tort le cas précondition structurée.
3. Après RED observé seulement : raccorder le parser intégré par cas/invocation,
   conserver population et causes, enlever la garde uniquement pour les entrées
   effectivement raccordées. Aucun code GREEN anticipé dans cette remise.
4. Le lead observe GREEN, non-régression puis organise la revue indépendante.

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_context_collector_contract.py' -v
```

Après GREEN, non-régression proposée, séquentiellement :

```text
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_core -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_context -p 'test_*.py' -v
C:/Python313/python.exe -B -m unittest discover -s scripts/tests/verification_scenarios -p 'test_*.py' -v
```

Les résultats historiques (142 succès/2 SKIP core, 28 contexte, 31 scénarios)
restent attribués au lead/rapport73, pas à ce diff. Les anciennes assertions de
blocage sans transport restent à préserver ; ne pas retirer globalement la garde
pour satisfaire le nouveau nominal.

## Cas et rattachement QA

- A3/A5 : observation absente/double/tronquée, identité ou cible étrangère,
  échange entre cas, stdout global seul, cas omis/dupliqué/étranger, découverte
  discordante/dupliquée. Code4, bruts conservés.
- A4/A5 : deux invocations JUnit, identités distinctes ; observation rejouée depuis
  l'autre invocation refusée. Cible Unicode, espaces, quotes et antislash exacts.
  Le faux adaptateur transmet les six variables explicitement. Cela ne valide pas
  les adaptateurs Windows/Linux de production.
- A5/A6 : cas CPU sans observation GPU ; précondition UNAVAILABLE + skip cohérent
  →3 ; skip ordinaire sans preuve jamais assimilé à ce cas ; UNAVAILABLE sans
  arrêt natif représenté par skip →4. Une assertion ailleurs conserve FAIL/5 et
  les deux causes ; protocole + assertion conserve FAIL/4 et les deux causes.
  L'ordre des cas est permuté pour ces deux compositions.
- A6 : enfant incompatible code8 sans failure →4 ; interruption avec code natif
  -9 →130 et bruts/logs préservés. JUnit absent/tronqué n'est pas une preuve de
  précondition. Aucun faux succès tiré d'une seule étiquette renderer.
- A7/A8 : rapport readonly sous les sentinelles existantes, empreintes des bruts,
  mutation JUnit relue →STALE/4 sans relance ni écriture. Les tests list/plan et
  filtrage d'environnement acquis restent dans la non-régression, pas dupliqués.

Les fixtures ne recopient pas le parser : des dictionnaires d'entrée synthétiques
sont produits et le vrai parser déjà intégré sert de contrôle nominal. Le GREEN
devra réutiliser ce parser ; la revue source reste nécessaire pour le confirmer.

## Ownership, erreurs et limites

Chaque test possède ses répertoires temporaires avec espaces, faux binaires
inertes, XML, stdout/stderr et sink. `RuntimeFixture` et ses doubles existants sont
réutilisés par sous-classes locales. Seul le vocabulaire `owned-fake` est accepté ;
aucun service natif ou fallback. Le cas CLI reçoit explicitement runner, adaptateur
et lecteur de révision factices. Une sentinelle Popen complète ce confinement.

Le sink possède les fichiers ; le collecteur devra consommer le system-out d'un
seul cas avec identité de son invocation, sans cache inter-cas ni contexte GPU
possédé/emprunté. Les garanties de capture/nettoyage du runner sont inchangées.
Une collecte partielle ne doit pas effacer les assertions déjà attribuées ni les
logs ; priorité interruption > protocole > oracle > précondition > inconclusif.
Un seul écrivain/bundle, aucune mutation de source concurrente.

Serena : racine canonique ARTIST confirmée ; overview Python refusé car seul le
LSP C++ est actif. Analyse des callers faite par recherches exactes et lectures
ciblées sur les fichiers identiques de cette base. Aucun résultat symbolique
Python revendiqué. Pas d'activation/reconfiguration de l'instance partagée.

Contrôle auteur effectué : parsing AST sans import/exécution du module, syntaxe
valide et 20 méthodes recensées. Premier appel Context Mode mal échappé corrigé,
sans exécution de tests ; ce n'était pas un défaut du candidat. Contrôle de diff
et identité du commit à communiquer dans la restitution finale.

**Exécution de tests auteur : zéro. RED/GREEN/non-régression NON_EXECUTE ici**,
à réaliser par le runner lead. C++ NON_COMPILE, GL/rendu/benchmark NON_EXECUTE,
aucune installation, réseau ou ressource cloud. Windows/Linux natifs non qualifiés.
La garde de collecte est encore active : aucun pipeline complet livré par cette
seule tranche RED_READY.
