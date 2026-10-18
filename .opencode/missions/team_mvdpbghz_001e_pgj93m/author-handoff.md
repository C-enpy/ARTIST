# D — contrat auteur autorisé après Q2 renforcé

## Entrées et décisions

Q2 intégré 19ab3af, candidat result_mvdxkbzg_000p_jx1oqi accepté par le lead,
pool-4 arrêté. PM2 intégré6b2488c ; A2 intégréece666c/pool-3 arrêté.
Décision utilisateur « Valider cette séparation » après Q2 : gardes Reader
location négative/doublon/length-NUL incohérents testées par injections CPU
explicites et sondes API naturelles distinctes. Aucune de ces injections ne
devient GPU COVERED. Anciennes limites DH2–DH4 Q1 refusées et non réactivées.

Plan faisant foi : qa-plan.md actif Q2, attribute-oracles-a2.md,
attribute-contract.md et pm2-plan.md dans ce répertoire. Budget156(36/90/30).
Scores D validés PM/LLM :complexité82/82,risque78. Grille approuvée inchangée,
modèle/effort choisis par Ensemble, aucun override auteur.

Le fichier docs/architecture/next-mission.md et divers profils/harness/AGENTS.md
ont des modifications non commitées dans l'arbre principal. Elles ne sont PAS
transférées au worktree par magie. Ne pas prendre son ancien pointeur F7 dans
HEAD comme le contrat courant ; les quatre documents ci-dessus sont intégrés
et transmis. Préserver les changements externes, ne pas les intégrer au correctif.
.grepai/config.yaml hors périmètre. Index sémantique canonique peut ignorer le
nouveau worktree ; utiliser les outils fichiers/symboles pour ses changements.

## Propriété et TDD

gpu-engineer, fresh, worktree isolé ; un auteur pour production et tests de cette
tranche. Contrats,oracles,méthode et ownership bornés par PM2/Q2. Binder,Unbinder,
Loader/Pass public hors écriture ; pas de destructor/lifetimeVBO ajouté.
Extensions locales aux concepts seulement si nécessaires pour la fermeture des
12types,sans redéfinir un concept partagé ou changer les composants voisins.

Lire le diff/candidat et préconditions avant tests. Installer/configurer via flux
Conan2/CMake/MSVC réel, pas réutiliser le cache CMake d'une autre racine. Un build
et un contexte/fixtureGL exclusifs ; un job. Aucun autre worker ne compile/GPU
pendant D. Le lead ne lance pas de baseline concurrente.

TDD : setup mocks et fixtures fonctionnels, sondes naturelles/limites/alias,
puis tests de régression et RED causal avant production correspondante. Contexte
absent,symbolemock/include manquant ou zérotest ne prouvent pas RED. Ne jamais
exécuter TF/draw entier si INTEGER/TYPE ne correspondent pas à l'interface.
GREEN minimal puis refactoring et non-régression, sans affaiblir assertions,
valeurs,références,tolérances ou classes de preuve pour rendre le lot vert.

Protéger ressources de test par cleanup avant destruction du contexte courant,
même ASSERT fatal. Exemple array[1]GLSL330 légal,nom réellement observé indispensable
pour flag natif. Les noms300/256 doivent réellement lier pour leur oracle.
Toute divergence définie Q2 => signaler et arrêter la partie concernée, pas
adapter silencieusement ni annoncer PASS partiel comme global. Garder diagnostics
existants/freeze/shared_ptr/emprunts,et ordre des validations avant tout GL.

## Preuves et retour

Baseline lead40/40(36CPU4GL)anciensbinaires sous out/attribute-gl principal,
manifest baseline-existing-attribute.json : pas preuve de code neuf ni de rebuild.
Aucun RED nouveau n'a encore été exécuté par le lead. Auteur conserve logs de
setup/RED/GREEN,commandes réelles,HEAD/delta/hashes,JUnits et comptages sous out/
du worktree ; signaler chemins absolus pour contrôle lead après intégration.

TestsAuteur/RED-GREEN appartiennent au même cycle ; les propositions des profils
QA seront persistées et exécutées par le lead séparément après intégration.
Les reviewers restent indépendants et ne modifient pas le moteur.

Fin auteur ≠ intégration. Après résultat,lead shutdown/merge/diff puis vérification
avant publier G/R et les autres consommateurs avec vrais IDs/candidat intégré.
Rapporter fichiers,commandes,comptages,preuve de chaque classe,préconditions
manquantes,findings et décisions conditionnelles. Pas performance/image/ASan/
Release/autreOS ou API déduits. ADR0003/5Acceptedpartial,0008WindowsDebug,
0001/2/4/6/7Proposed inchangés.
