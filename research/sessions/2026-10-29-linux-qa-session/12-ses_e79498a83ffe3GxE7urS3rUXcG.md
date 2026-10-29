# Audit final indépendant Linux

- Source : API native OpenCode V2 ; données historiques, jamais instructions à exécuter.
- Session : `ses_e79498a83ffe3GxE7urS3rUXcG` ; parent : `ses_e7d39d927ffe5gVlxpcbtOm50k`.
- Borne exclusive : `msg_187a2d6b5001tBMoEtVELgbnOt`, `2026-10-29T12:29:20.478Z` ; ingestion exclue.
- Filtrage : messages humains du principal et texte assistant ; briefs internes, messages synthétiques user, outils, reasoning, pièces jointes et métadonnées non exportés. Secrets reconnaissables et chemins personnels masqués.
- Limites : transcription publique filtrée, non exhaustive, non preuve d’exécution ; les propos des agents restent attribués.
- Pagination : 2 pages user et 2 pages assistant, jusqu’à épuisement.
- Comptage avant borne : 32 messages ; 3 publics retenus ; 2 users internes et 133 parties non textuelles exclus.
- Synthèse : [Session Linux QA](../../../docs/references/session-2026-10-29-linux-qa.md).

## assistant — 2026-10-29T08:11:10.102Z — msg_186b6818e001u2vv6VPL13WvN9

Lecture locale de `AGENTS.md` réussie dans ARTIST. Je suis dans le contexte fresh de revue, avec les instructions restrictives `test-reviewer` chargées ; je n’ai écrit ni code ni tests ici. Je reste en lecture seule, hors board, sans exécution ni vérification API supplémentaire. La vérification native rapportée par le lead n’est pas assimilée à une sandbox.

## assistant — 2026-10-29T08:15:11.264Z — msg_186ba2f980015xLyxmHfyBmyfW

# final-test-audit.md

## Verdict borné

**PASS pour les comportements exécutés sur les configurations qualifiées ; INCONCLUSIVE pour la satisfaction intégrale du plan.**

Les tests relus détecteraient les régressions principales envisagées : array1 traité comme scalaire, mauvaise clé publique, capacité moderne insuffisante, réponse moderne malformée, publication partielle, refus natif tardif, renderer erroné, environnement ASan hostile et code enfant perdu.

**Aucun défaut bloquant démontré dans les campagnes moteur finales.** Une lacune de couverture de fraîcheur demeure dans le contrôleur : les shaders consommés ne participent pas à son contrôle d’identité. Elle interdit de qualifier ce contrôle de protection complète contre tout changement du candidat.

Les limites historiques RED, l’incrémentalité Windows inexpliquée et les configurations exclues restent conservées. Aucun PASS global moteur, performance ou sûreté exhaustive.

## Admission, indépendance et restitution

- Lecture locale de `AGENTS.md` réussie ; skill `artist-build-validation` chargé.
- Contexte fresh, hors board ; aucun code ni test écrit par ce reviewer.
- Lecture seule respectée : aucune édition, commande shell, compilation, exécution de test, API session/grant ou délégation.
- Aucun rapport narratif, transcript ou justification privée d’auteur utilisé.
- Vérification native du profil attribuée au lead, conformément au mandat ; pas de vérification API supplémentaire ni de garantie de sandbox déduite.

Transport découvert puis essayé une seule fois :

```text
execute → tools.team_message(to="lead", ...)
This session is not in a team.
```

Restitution native utilisée en conséquence. **Aucun fichier écrit ici** ; persistance et confirmation de réception restent au lead.

## Pin observé

HEAD local lu dans `.git/refs/heads/main` :

```text
d261848d280988155f0deb327cd1686e8d9a81e9
```

Le pin historique du profil et celui du skill ne correspondent pas au dépôt courant : **ni appliqués ni réécrits**.

Identités distinguées :

- candidat intégré des derniers tests Python : `1532b78d96489923773e7b2a8b355dc91f94345c`, selon l’entrée neutre ;
- candidat exécuté par les six campagnes moteur : `78e7b74989dd463cddf14080916e03b60405640d` ;
- `0a4a671` : état antérieur à la réparation du gate, pas substituable au candidat final.

La conservation du runtime/configuration après `78e7b74` est une digestion fournie par le lead, cohérente avec les diffs livrés ; je n’ai pas recalculé les hashes ni un diff Git.

Statuts documentaires conservés : ADR-0001/2/4/6/7 Proposed, ADR-0003/5 Accepted/partial, ADR-0008 vérifié dans son périmètre Windows Debug. Le registre courant précise notamment ADR-0004 Proposed/partial. Aucun nouvel ADR adopté.

## Preuves examinées et attribution

Racine commune ci-dessous : `out/logs/mission-linux-qa/`.

| Environnement | Runner | Preuves primaires | Résultat |
|---|---|---|---|
| Windows normal | lead | `gate-green-windows-normal.xml`, découverte et log associés | 589 : 513 CPU + 76 GL |
| Windows ASan | lead | `gate-green-windows-asan.xml`, découverte et log associés | idem |
| WSL normal | lead | `gate-final-linux/20261028T201504Z-8aba0173a35247e4a58c624787680584/` | idem |
| WSL ASan | lead | `gate-final-linux/20261028T201850Z-5bd88de1cef04e518c968dd062b0827a/` | idem |
| Ubuntu CI normal | GitHub runner | `ci-40760506988/linux-normal-40760506988-1/` | idem |
| Ubuntu CI ASan | GitHub runner | `ci-40760506988/linux-asan-40760506988-1/` | idem |

**executed — attribution aux runners, pas au reviewer :**

- Les six XML indiquent zéro failure et skip ; les recherches bornées n’ont trouvé aucun élément `failure`, `error` ou `skipped` dans ces résultats finaux.
- Les découvertes contiennent les cas discriminants Attribute et pixels, présents également dans les XML.
- Les manifestes Linux consignent commandes, populations concordantes et retours CTest zéro. Les deltas suivis consultés sont vides.
- Le recoupement intégral par nom, les 149 hashes par run et la bijection Windows/Linux après le seul cast GLM sont **les vérifications exécutées rapportées par le lead**, dans `gate-final-reconciliation.json` et l’entrée neutre ; je ne les ai pas recomptées.
- WSL : GCC 14.2.0, CMake 3.28.3, Conan 2.12.2, Ninja `1.13.0.git.kitware.jobserver-pipe-1`, noyau WSL2 x86_64 consignés.
- Les XML GL Linux montrent Mesa/llvmpipe, OpenGL 4.5 core, Mesa 25.2.8. Windows montre NVIDIA RTX 3090, OpenGL 3.3.0, pilote 621.01.
- Les canaris WSL/CI consultés donnent sain **0**, UAF **86**, avec diagnostic explicite `heap-use-after-free` et politique canonique, `detect_leaks=0`.
- Les commandes CI consultées portent bien `-fsanitize=address -fno-omit-frame-pointer`. L’exhaustivité 43 TU unit + 13 composants et les symboles/libasan WSL proviennent des digestions d’instrumentation du lead. Les hashes binaires WSL correspondent à ceux du manifeste exécuté.
- `qa-gaps-windows.log` et `qa-gaps-linux.log` établissent chacun **9/9 tests Python**, distincts des populations moteur.
- `qa-gaps-mutations.json` rapporte trois mutants en mémoire détectés. Aucune mutation n’a été exécutée par ce reviewer.

**static-presence :** assertions, fixtures, CMake/presets, Conan, workflow et trois diffs intégrés relus.

**inference :** sensibilité aux mutants mentaux décrits ci-dessous et finding F1.

## Obligations → preuves → mutants discriminants

Référence du plan : `.opencode/evidence/mission-linux-qa/qa-plan.md`.

| Obligation et lignes du plan | Input → mutant plausible → assertion discriminante | Verdict |
|---|---|---|
| **O1**, 34–36 : environnement/isolation | Configuration Linux → cache/toolchain Windows réutilisé → profil Linux GCC14, arbres distincts et configure réel observé. `test_install_precedes_configure_and_cpu_selection` contrôle l’ordre Conan/CMake. | **PASS borné** aux builds observés. Provisionnement physique sur I: non vérifié indépendamment ici. |
| **O2**, 38–40 : C++23 | `std::format` + `std::stacktrace` → simple drapeau C++23 sans linkage → sonde compile/link réelle. WSL échoue en natif puis réussit avec `stdc++exp`, sans suppression de sonde. | **PASS positif**. Rejet autonome d’une toolchain STL incapable non nouvellement qualifié ici. |
| **O3**, 42–44 : CPU | Population attendue → résultat vide/manquant/dupliqué/skipped → comparaison découverte/JUnit et négatifs de `reconcile`. | **PASS** pour les populations finales et les négatifs examinés. |
| **O4**, 46–48 : GL réel | Deux cas GL dont le second sans contexte ou matériel → ne vérifier que le premier → `test_controller_requires_llvmpipe_context_in_every_graphics_case`, `test_linux_evidence.py:60–71`, exige l’échec sur le second. | **PASS logiciel Linux**. |
| **O5**, 50–52 : pixels | Valeurs 0 puis 0,5 → update ou draw omis → points fixes blanc/noir, image variant avec la valeur et readback uniform. `PreparedFloatUniformRenderTests.cpp:132–139,233–262`. | **PASS image ciblée** ; nouvelles mutations Linux omission update/draw non revendiquées. |
| **O6**, 54–56 : ASan | Parent hostile → options permissives propagées → vérification de chaque enfant et parent inchangé, `test_linux_evidence.py:73–86`, plus canari réel 0/86/diagnostic. Retirer seulement l’instrumentation moteur serait également visible dans les contrôles TU/binaires fournis. | **PASS borné**, pas preuve de fuites ni de dépendances instrumentées. |
| **O7**, 58–60 : fraîcheur/erreurs | Enfant échoue avec ancien résultat disponible → code perdu/artefact réutilisé → création exclusive, arrêt avant CTest, retour 8 et manifeste FAIL. Changement CMake après tests → garde d’identité. | **PASS sur ces scénarios ; INCONCLUSIVE complet**, voir F1. |
| **O8**, 62–64 : causalité/TDD | GLEW expérimental sans extension annoncée → ancien gate admis → appels modernes interdits, puis échec attendu capturé avant correction. | **PASS pour ce RED identifié** ; historique classification incomplet conservé. |
| **O9**, 66–68 : remise | Restitution collaborative indisponible → faux succès de transport → refus exact conservé et présente remise native. | Transport équipe **indisponible** ; persistance/réception par le lead encore nécessaires. |
| **O10**, 70–72 : mutualisation | Adaptateurs divergents → assertions/oracles différents → CMake/Conan/CTest, vérificateur ASan et fixtures communs, campagnes Windows et Linux distinctes. | **PASS borné** au diff et aux campagnes. |

### Complément Attribute A1–A8

- **A1/A5 — clés et classification**, plan 119,123 : `AttributeComponentTests.cpp:424–458` énumère indépendamment la clé legacy du même programme. Scalaire, array1 et array2 sont comparés à leur classification attendue. Ajouter un alias ou revenir à `size > 1` échouerait. Les XML montrent `qaArray` sur Mesa et `qaArray[0]` sur NVIDIA, sans imposer une orthographe fournisseur.
- **A2 — capacité**, plan 120 : `AttributeReaderTests.cpp:343–395` couvre core seul, extension seule, absence de capacité malgré pointeurs, chacune des trois entrées manquantes et flag GLEW expérimental sans annonce effective. Les négatifs exigent zéro appel moderne et le résultat legacy.
- **A3/A4 — association et réponses**, plan 121–122 : attentes sur programme, `GL_PROGRAM_INPUT`, nom exact et index non ordinal ; fautes de propriétés, longueurs/NUL, type/taille/location et suffixes invalides dans `AttributeReaderTests.cpp:305–339,398–458`. Retirer une validation doit produire absence du diagnostic attendu ou publication indue.
- **A6 — refus natif**, plan 124 : `AttributeComponentTests.cpp:687–738` prépare les ressources avant snapshot puis vérifie diagnostic, cible, leurre, binding, usage et état vertex pertinent. Écrire puis lever serait observable. Le témoin scalaire à `742–761` vérifie l’upload exact.
- **A7 — staging**, plan 125 : `AttributeReaderTests.cpp:462–489` part d’une génération non vide, stage une entrée valide puis rencontre un index invalide. Identité du pointee, métadonnées et valeur anciennes sont conservées ; aucun préfixe publié ; succès ultérieur possible. Une publication anticipée échouerait.
- **A8 — frontière**, plan 126 : la nouvelle réflexion reste statiquement au Reader. Les consommateurs existants sont exécutés. Aucun nouveau rollback CPU du wrapper n’est exigé ou déduit.

**PASS borné A1–A7 sur les scénarios exécutés ; A8 étayé par présence statique et non-régression, pas par mesure de coût.**

## Finding

### F1 — Moyenne : le négatif d’identité ne couvre pas les shaders consommés

**Localisation :** `scripts/linux-evidence.py:63–70,114–115` ; `scripts/tests/test_linux_evidence.py:19–26,45–46,88–100`.

**Références :** plan O7, lignes 58–60 et 90 ; skill, `references/validation-levels.md:12–18`.

**Fait — static-presence :** l’identité couvre des fichiers explicitement listés, certains suffixes C++/texte sous `ARTIST` et les exécutables. Elle ne couvre pas les `.vert`/`.frag` de `test-datas`, pourtant chargés par les tests GL. Le négatif ajouté change uniquement `CMakeLists.txt`.

**Scénario discriminant — inference :**

1. Le contrôleur capture son identité après build.
2. Un shader consommé est modifié avant la fin du run, par exemple par ajout d’un commentaire conservant le comportement.
3. Tous les tests et pixels restent verts.
4. L’identité calculée reste identique : le contrôleur peut annoncer PASS malgré une entrée graphique différente du candidat annoncé.

**Assertion manquante :** le même scénario de fixture, avec changement d’un shader consommé, devrait imposer retour non nul et manifeste FAIL d’identité.

**Impact :** la garde ne démontre pas la fraîcheur de toutes les entrées du rendu. **Cela ne prouve pas qu’un shader a changé pendant les six campagnes**, et ne les invalide pas rétroactivement.

**Vérification minimale proposée :** compléter la fixture temporaire existante avec un shader et sa modification contrôlée ; constater le défaut attendu, puis vérifier le périmètre d’identité corrigé. Aucun besoin de nouvelle infrastructure ni de mutation moteur sur disque.

**Destination :** `DEFER_TO: test-engineer` pour le négatif ; `DEFER_TO: qa-planner` pour disposition explicite du périmètre O7. Toute correction du helper relève de son propriétaire sous mandat distinct.

**Statut :** gap de couverture inféré, non reproduit ici ; pas un FAIL exécuté. À disposer avant un PASS intégral O7.

## Registre inverse, limites et décisions

- Les trois nouveaux tests Python correspondent respectivement à **O4**, **O6**, **O7** ; aucun n’est présenté comme preuve GPU.
- Tests du lanceur → O1/O6/O7 ; vérificateur/canari → O6/O8 ; tests Reader → A1–A5/A7 ; composants natifs → A1/A5/A6 ; pixels et consommateur → O5/A8.
- Le changement d’oracle stacktrace conserve le contrat de retour propriétaire et l’égalité exacte de contenu sur la branche compilée (`TraceableExceptionTests.cpp:50–82`) : pas d’affaiblissement constaté.
- Les répétitions de scénarios array1 à plusieurs frontières ont une utilité distincte : description, classification, puis refus natif. Elles ne constituent pas trois preuves indépendantes d’un même comportement.
- Aucun nouvel orphelin identifié dans les ajouts relus. L’inventaire historique complet n’a pas été reclassé test par test.

Limites maintenues :

1. Premier RED classification causalement lisible dans `attribute-correction-red/red.xml`, mais delta complet non hashé à chaud : **INCONCLUSIVE pour sa provenance complète**, sans reconstruction rétrospective.
2. RED gate ultérieur : `gate-red.patch`, identité et XML établissent **513 CPU, un échec attendu** avant production.
3. Violations d’accès Windows incrémentales archivées ; clean à sources inchangées puis campagnes finales propres. Cause exacte du suivi de dépendances non entièrement expliquée.
4. Fallback 3.3 sans information moderne et nom ambigu : gap de qualification explicite, pas garantie universelle.
5. Pas de qualification Release, UBSan, performances, fuites exhaustives, GPU matériel Linux ou autres backends.
6. La lecture des preuves n’est jamais une exécution par ce reviewer.

### Actions proposées au lead

- Persister cette remise sous le nom demandé et confirmer sa réception native.
- Disposer F1 sans élargir silencieusement le PASS O7.
- Aucun rerun moteur demandé uniquement pour cette restitution documentaire.
- Si F1 est corrigé : exécuter les tests Python concernés sur les deux OS, conserver identité/commande/résultats, puis ré-audit ciblé. Si le candidat moteur/configuration change, renouveler les preuves impactées.

Commandes possibles **au runner seulement**, non exécutées ici :

```text
python -B scripts/tests/test_linux_evidence.py -v
python -B scripts/tests/test_build_linux.py -v
python -B scripts/tests/test_verify_asan.py -v
```

**Conclusion :** les principaux défauts plausibles du diff disposent d’oracles discriminants et de preuves exécutées attribuées aux runners. Aucun blocage moteur démontré ; réserve O7 et limites historiques empêchent une certification exhaustive ou un PASS global.

## assistant — 2026-10-29T08:20:24.884Z — msg_186bef8ab001kc0R1f9yiuOdVc

# Ré-audit ciblé F1 — fermeture

## Verdict

**PASS borné — F1 fermé** sur `f58a49f4261e018749c59b0d8aab9dd7e85ba53e`.

Le défaut précédemment inféré est maintenant reproduit avant correction, puis détecté après réparation. Aucun nouveau finding dans ce delta ciblé. Les autres limites du rapport initial restent inchangées.

Même contexte indépendant, lecture seule : aucune édition, exécution ou délégation par ce reviewer ; aucun nouvel essai `team_message`.

## Candidat et preuve

HEAD local lu : `f58a49f4261e018749c59b0d8aab9dd7e85ba53e`.

Sources consultées sous `out/logs/mission-linux-qa/` :

- `shader-identity-integrated.diff` et `.json` ;
- `shader-identity-red.patch`, `.json`, `.log` ;
- `shader-identity-green.json` ;
- `shader-identity-green-{windows,linux}.log`.

Les manifestes RED/GREEN/intégration donnent le même hash du test :

```text
847f28936a81f057945a8aa84c99608dc4e359242d1b2e0c3142e4b249c23feb
```

Le helper RED conserve son ancien hash ; GREEN et intégration indiquent :

```text
b4b3050fd770967283c4cd845089eab7dc778270fdc0e038955b1696a65fdbd1
```

Ces empreintes sont celles consignées par le lead, non recalculées ici.

### executed — runner : lead

- **RED :** 10 tests, exactement deux sous-échecs, `.vert` et `.frag` modifiés. Le retour reste incorrectement zéro : `AssertionError: 0 == 0`. Le patch RED ne modifie que les tests.
- **GREEN Windows :** 10/10, `OK`.
- **GREEN Linux :** 10/10, `OK`.
- Les deux changements de shader produisent désormais le diagnostic d’identité attendu ; les témoins stables restent verts.

Il s’agit de tests Python du contrôleur avec fixtures, **pas de tests OpenGL réellement exécutés par ces fixtures**.

## Scénario discriminant et qualité du test

Référence : plan QA **O7, lignes 58–60 et 90**.

`test_linux_evidence.py:104–118` couvre chaque extension avec un témoin stable et un cas modifié :

```text
XML GL vert produit
→ shader modifié dans la fixture
→ retour non nul + manifeste FAIL + diagnostic exact d’identité
```

L’assertion `counts['opengl']['executed'] == 1` vérifie que le rejet survient après le résultat graphique simulé, pas par défaut de population. La modification intervient explicitement après écriture du XML (`:46–48`).

**Mutation mentale :** retirer l’inclusion de `test-datas`, exclure `.frag` ou supprimer la comparaison finale ferait survivre un changement ; le sous-cas négatif correspondant l’arrêterait. Le RED fourni confirme déjà le premier scénario.

Les assertions ne sont pas assouplies entre RED et GREEN. L’identité réelle du contrôleur n’est pas mockée ; seule l’exécution enfant est simulée, ce qui convient à cette frontière.

## Périmètre et risques directs

**static-presence :** `linux-evidence.py:69–72` inclut récursivement tous les fichiers de `test-datas`, sans filtre d’extension, avec SHA256 du contenu. La comparaison finale reste obligatoire avant PASS (`:116–119`).

**inference :**

- les shaders imbriqués sont couverts ;
- les ajouts/suppressions persistants de fichiers changent également les clés du dictionnaire ;
- une erreur de lecture remonte vers le chemin FAIL existant.

Ces derniers scénarios ne sont pas présentés comme nouvellement exécutés. Aucun coût ou gain de performance n’est déduit de l’extension du hashage.

## Portée de fermeture

F1 est fermé pour **l’omission des entrées `test-datas` dans la garde d’identité**, avec preuve RED/GREEN Python et correction intégrée.

Les nouvelles campagnes WSL/CI avec manifeste renforcé restent **NOT_RUN dans les artefacts examinés**. Les anciens résultats 589 ne sont ni transférés ni rebaptisés exécutions de `f58a49f`.

Aucun complément de test ou ré-audit supplémentaire demandé pour ce petit correctif. La qualification runtime des nouvelles campagnes reste une preuve séparée, à restituer par le lead.
