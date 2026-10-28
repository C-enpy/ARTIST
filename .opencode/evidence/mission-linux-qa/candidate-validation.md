# Candidat intégré — contrat et preuves pour contrôles indépendants

## Mise à jour prioritaire — correctif final exécuté

Candidat moteur `78e7b74989dd463cddf14080916e03b60405640d`. Le finding GLEW du candidat précédent décrit ci-dessous est corrigé et fermé en ré-audit indépendant. Delta `out/logs/mission-linux-qa/gate-repair-integrated.diff`, identité correspondante. RED lead identifié à chaud, Reader inchangé lors du RED ; puis GREEN Windows normal/ASan à589 chacun.

WSL normal/ASan et CI Ubuntu `40760506988` au même candidat :589 chacun=513CPU+76GL, aucun failure/error/skip ; manifestes PASS, découvertes/XML et noms recoupés. Résumé `out/logs/mission-linux-qa/gate-final-reconciliation.json`, quatre archives Linux sous `gate-final-linux/` et `ci-40760506988/`. 149 hashes source/config par run correspondent au candidat ; deux fichiers Windows ont seulement CRLF, comparés aux blobs Git exacts. Les six configurations ont les mêmes cas après le seul cast GLM déjà documenté. Les deux passages Windows sont sous `gate-green-windows-{normal,asan}.*`.

La revue C++ a fermé le finding à la frontière revue, sans exécuter de test. Les trois derniers compléments du generator concernent seulement les tests du contrôleur de preuves ; voir `gaps-integrated-candidate.md`. Audit final fresh encore à recevoir ; aucun PASS global de mission à ce stade. La section suivante est le **dossier historique du candidat précédent**, pas le statut actuel.

## Identité et portée

Candidat intégré : `0a4a6713a6ab9fad0d52108c8b52432059015fb6`. Le workflow/helper est intégré à4494301, la correction Attribute au candidat ci-dessus. La référence pour le diff de correction est23b1934 ; limiter la lecture au C++/tests concernés. Les modifications locales préexistantes de profils/harness/docs ne font pas partie du candidat et ne doivent pas être éditées ou assimilées à son comportement.

Contrat adopté : `.opencode/evidence/mission-linux-qa/attribute-correction-contract.md`. Plan : `qa-plan.md`, complémentA1–A8. Clés natives legacy exactes et API inchangées ; nom moderne temporaire pour classification seulement, gate version/extension effective et fonctions disponibles ; fallback historique si route non disponible, rejet du staging si réponse moderne incohérente. Refus avant mutation nativeGL ; aucun rollbackCPU supplémentaire promis au wrapper. Pas de garantie universelle de pilotes3.3 ambigus sans route moderne.

StatutsADR conservés, OpenGL seul backend. Pas de performance, Release, UBSan, GPUmatérielLinux ou absence exhaustive de fuites à conclure.

## Chemins du candidat

- Production : `ARTIST/include/graphic/opengl/pipeline/component/pass/AttributeReader.hpp`.
- CPU : `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/AttributeReaderTests.cpp`.
- Mocks : `ARTIST/tests/mock/opengl/glFunctionMock.hpp`.
- Contexte natif : `ARTIST/component-tests/src/graphic/pipeline/AttributeComponentTests.cpp`.
- Helper intégré : `scripts/linux-evidence.py` et `scripts/tests/test_linux_evidence.py`.
- Workflow : `.github/workflows/cmake-single-platform.yml`.

## Preuves effectivement disponibles, sans confondre les candidats

Archives brutes copiées sous `out/logs/mission-linux-qa/attribute-author/`. Elles portent le candidat auteur ddab33a, antérieur au commit intégré, et ne doivent pas être rebaptisées exécutions du lead. Les quatre fichiers C++/tests exécutés sont identiques, hashes recoupés après intégration ; les autres entrées de configuration doivent être considérées à leur portée. Les validations du candidat combiné et la CI finale ne sont pas encore acquises dans cette version du dossier.

Recoupement XML/inventaires et26hashes du manifeste effectué par le lead : quatre runs à588cas uniques=512CPU+76GL, zéro failure/error/skip et noms exactement égaux à la découverte de leur plateforme. Ensembles normal/ASan identiques ; correspondance Windows/Linux bijective après le seul remplacement de `(glm::qualifier)0` par0,144différences brutes, aucune collision ou absence. Depuis581 : six casCPU et un casGL ajoutés, aucun cas retiré.

XML finaux :
- Windows normal : `attribute-author/windows-msvc-debug-final.xml`.
- Windows ASan : `attribute-author/windows-msvc-debug-asan-final.xml`.
- Linux normal : `attribute-author/linux/final-normal.xml`.
- Linux ASan : `attribute-author/linux/asan-runs/20261028T190210Z-6d8394186dd34dc68889671c8685e9a8/ctest.xml`.

Les chemins relatifs ci-dessus sont sous `out/logs/mission-linux-qa/`. Le manifeste et les fichiers de découverte sont dans le même dossierarchivé. Contextes observés dans les archives : Mesa25.2.8/llvmpipe4.5core et Windows/NVIDIA3.3core ; vérifier les données plutôt que déduire une capacité du nomGPU.

RED natif : `attribute-correction-red/{red.xml,red.log,red-build.log}`,1cas en échec sur isGLArray avec clés natives et témoin scalaire passant, compile/link réussis. REDCPU : `attribute-author/linux/cpu-red.xml`,18cas dont2failures,0error/skip. La première faute de compilation de test n'est pas unRED. **Limite d'identité historique : le hash complet du delta du premier RED n'a pas été enregistré à chaud ; aucun hash final ne lui est attribué rétroactivement.**

Canaris0/86 et diagnostic attendu sont requis selon le plan ; inspecter leurs journaux à leur candidat et conserver detect_leaks=0. Les résultats initiaux WSL/CI à3failures et leurs manifestesINCONCLUSIVE erronés sont conservés séparément ; le reporting corrigé a un test contrôleur donnantFAIL+75/3+exit8 et6testsPython exécutés par le lead sur chaqueOS.

## Empreintes des quatre fichiers intégrés et exécutés

- Reader : `342db35fbafb7977c34ea0f140fcf767f3f70234c51c8fba353f7ddf88358903`.
- TestsCPU : `bccd868a1b6732eb7d75cf7b2cf81a58d13ac36082758a179ce223d1389f63c9`.
- Mocks : `05b49a382c6d1f52af58541f42c093d806a547a7733196157534c7103be87358`.
- Composants : `6d790f05dc929affae9d21ee2f76845f34bdedb922f10470075d620e8395d4a7`.

Le miroir Linux auteur pointe ddab33a, diff suivi vide au recoupement ; un cache Python non suivi était présent. Ne pas le décrire comme un checkout absolument sans fichiers non suivis.

## Mandat des contrôles indépendants

Revue C++ ciblée : gates, bornes/conversions, cohérence des requêtes et nom, nettoyage/staging, contrats des consommateurs ; pas de production ou tests modifiés. Recherche QA de gaps : défaut plausible, scénario discriminant avant conclusion forte, KEEP/MODIFY/REJECT/DEFER, propositions seulement, aucun runner autonome. Seuls le lead ou un auteur autorisé persisteront et exécuteront les propositions acceptées.

La revue finale test-reviewer doit être fresh et hors board d'implémentation, après gaps intégrés et validations finales. Ne pas lui fournir les justifications privées des auteurs. L'admission exige le profil réel, son contexte, ses permissions et une voie de restitution utilisable. Une consultation de rafraîchissement des profils via API native a été refusée au lead ; ce refus n'est pas répété et ne confère aucun droit supplémentaire. La qualification finale d'admission reste à résoudre par les voies autorisées et preuves encore valides, sans substituer un profil ou fabriquer un audit.

Verdicts bornés : les archives auteur recoupées soutiennent leGREEN des configurations effectivement exécutées ; l'acceptation globale, les revues/gaps et la CI finale restent en attente. Zéro test ou preuve manquante n'est pasPASS.
