# F7 TE — oracles et note de tranche

## Provenance avant code et exécution

- Substitution explicite du profil test-engineer inerte par te-cpp-subst,
  cpp-engineer ; tâche task_mvco4dky_000w_30m1nv, périmètre des trois réserves.
- G1 : HEAD 17019e42ae4e3065651fc09c1dc544ac3f66e316 ; delta vide,
  untracked 0. SHA256 agrégé de 121 fichiers suivis ARTIST/include, tests,
  component-tests (ordre git ls-files, chemin + NUL + octets + NUL) :
  d63d34af77ea1ce74d8f575af9f7e3f48b616725b68ae90e7851401a609f6c74.
- Worktree te-cpp-subst initialement detached ; branche annoncée créée avec
  git switch -c sous autorisation du lead. Aucune écriture canonique/source.
- Baux exclusifs accordés : build/cache/tooling Conan canonique ; un job,
  aucun GL natif prévu. Nouveau fichier de test : aucun (extensions de trois TU
  existantes). Une reconfiguration CMake explicite confirmera leur découverte.
- AGENTS.md, skills artist-cpp-contracts, artist-build-validation, recherches
  GrepAI/QMD consultés. Serena sur le canon ARTIST, overview tests vide : lecture
  ciblée du worktree autoritaire ; aucune preuve LSP/templates revendiquée.

## Oracles écrits avant le premier run

1. **T1 KEEP** : move-assign Loaded->Loaded de Uniform<MockOpenGL>.
   Le contexte source (identité adresse) est transféré, jamais copié ; la valeur
   de son sous-objet de base any reste celle de la source. Compteur de copies du
   payload any remis à zéro avant assignation, attendu 0 ensuite. getContext de
   la source lève ERROR::UNIFORM::FREED_CONTEXT. La destination garde l'identité
   et la valeur après destruction de la source. La référence de destination
   antérieure est invalidée et ne sera jamais déréférencée après assignation.
2. **T2 MODIFY** : set<float>(shared_ptr<float>(0.5f)) doit appeler exactement
   MockSetter<float>::mockOn(Ref(context)) Times(1), jamais
   MockSetter<shared_ptr<float>>. Le stockage doit être le même shared_ptr<float>
   avec la valeur 0.5f. TearDown vérifie/réinitialise chaque singleton touché.
   Avant l'oracle final, contrôle négatif **oracle-only**, pas RED causal moteur :
   attente volontairement inversée (float Times(0), shared_ptr<float> Times(1)).
   Attendu exit test=1, CTest=8, JUnit failure=1 avant summary ; échec attendu
   appel inattendu float + attente shared_ptr<float> non satisfaite en TearDown.
   Ensuite seuls les deux Times sont corrigés, aucune production modifiée.
3. **T3 KEEP caractérisation** : une première réflexion de staleUniform et
   reloadedUniform publie size=2. Une seconde réflexion size=3 avec deux noms
   reloadedUniform identiques publie le premier puis rejette le doublon par
   ERROR::PASS::UNIFORM::DUPLICATE_NAME. Attendu : map size=2, reloaded size=3,
   stale size=2 et identité de stale conservée. Cet oracle discrimine publication
   immédiate partielle vs rollback/publication atomique ; il caractérise le
   comportement intégré, sans adopter un nouveau contrat de génération.

## Invariants et limites prévus

- Propriétaire Uniform = unique_ptr du contexte ; getContext/get empruntent.
  Le déplacement invalide les anciens emprunts destination, conserve ceux du
  contexte source tant que le nouveau propriétaire vit. Aucun emprunt ne doit
  être déréférencé après destruction/remplacement de son contexte.
- Le payload de test vit dans std::any du vrai sous-objet UniformContext ; son
  compteur survit aux deux wrappers. Aucune modification du mock ni du moteur.
- Exceptions T3 : publication déjà faite conservée (garantie de base seulement,
  pas de rollback). Cas CPU monothread, aucune synchronisation inter-thread.
- GPU : aucun objet réel ni soumission/frame/fence de ces cas CPU ; aucune
  conclusion sur durée de vie GPU, rendu, ASan, performance ou autres plateformes.
- Contrôles de compilation existants inchangés ; aucun nouveau concept requis.
- RED moteur non attendu sur les caractérisations T1/T3 du code intégré. Un
  succès initial n'est pas présenté comme un cycle RED causal de correction.

## Commandes, résultats, décisions finales

À compléter après exécution, sans modifier les prédictions ci-dessus.

- 01 configuration Conan2/preset : exit 0. 02 build un job : C1083 Invalid argument
  sur le chemin objet long d'AttributeReaderTests.cpp, aucun test exécuté, pas RED.
  Repli déjà autorisé par le lead : cmake -S worktree -B
  C:/Users/djden/source/repos/ARTIST/out/mission-f7/te-build, sans preset ni
  modification CMake persistée, mêmes variables Debug/MOCK_GL/ASAN=OFF.
  03 configure et 04 build ArtistLib_tests : exit 0. Cache/tooling canonique
  réutilisé sans copie ; MSVC 14.44, SDK 10.0.26100.0 ; Conan2.
- 05 discovery négative fraîche : 226 unitaires, dont les trois nouveaux noms.
- 06 contrôle négatif oracle-only : CTest exit 8, 1/1 échec attendu ; appel
  MockSetter<float> sursaturé et MockSetter<shared_ptr<float>> non appelé signalés
  avant summary. 07 exécution binaire directe identique : exit 1. Deux exécutions
  du même cas distinct, pas deux cas. Aucun défaut moteur ainsi revendiqué.
- Témoin JUnit : CLI --test-output-size-passed 65536 --test-output-size-failed
  65536 dans chaque commande CTest exécutée. XML négatif conservé byte-exact :
  49d749bf5a593841bf91c613163ad445552b8036a31da790d6232d12e8850b24.
- Oracle négatif source conservé sous oracle-negative/AttributeTests.cpp
  SHA256 32d273812cb95729793ec05e6139bf5f236d6e07fabbe9c58113b6e978f2f7c3 ;
  binaire conservé SHA256
  804ac2b9d734759d34fe51b552c719f7fd673298b7b697655cafa39f16448924.
  LastTest.log négatif également conservé. Après conservation : seuls les deux
  Times et le commentaire du nouveau test float sont corrigés (0/1 -> 1/0).

## Clôture (mêmes oracles, aucune production modifiée)

- T1 KEEP : le nouveau test vérifie aussi que le compteur de copies est opérant
  (ASSERT_GT avant remise à zéro). Le sous-objet de base réel porte le std::any ;
  après transfert, zéro copie et source FREED_CONTEXT, destination lisible après
  destruction source. Preuve : final-selected + final-unit, tag executed CPU.
- T2 MODIFY : oracle final exact float=1 / shared_ptr<float>=0, Ref(context),
  identité de stockage et valeur conservées. Preuve négative oracle-only 1 échec
  avant summary, puis final-selected + final-unit PASS, tag oracle CPU exécuté.
- T3 KEEP caractérisation : doublon après préfixe publié, reloaded size3 et stale
  size2/identité conservée. Preuve final-selected + final-unit, tag caractérisation
  CPU exécutée. Atomicité non adoptée ; aucun DEFER nécessaire pour l'oracle.
- 08/12/16 re-glob explicites ; aucun nouveau fichier source de test.
- 16 build final exit0, 17 découverte finale 226, 18 sélection3/3, 19 unit226/226,
  zéro échec/skip. Les noms XML226 égalent exactement la découverte226, sélection
  incluse. Les campagnes intermédiaires sont des répétitions des mêmes noms.
- JUnit final sélection SHA256
  cc524e2a5171cb3f219a6d2204abeb425b960a7a78abf09bd3e409c6d6d69d94 ;
  unit SHA256 538121e06d9487e0e96d7d09c70730e58a6c2916cbc443a4d3a92dacf8fda6ab.
- G2 source/test121 SHA256
  a55bd8fdc02f575151353116a4d10d3400ab410b529f73d35cdd115a44818a09 ;
  binaire final 1625c3795e62042fee3154197e72c98202c98b4ddd1f9b9483eba5464c264391.
  Identités et empreintes détaillées dans manifest.json. Production/mocks/CMake/
  fixtures inchangés, trois TU de tests seuls modifiés côté C++.
- Tous les runs sous bail exclusif ; annonce nommée build-window f7 / TE pour
  vérification16->19 seulement, omission de forme initiale signalée au lead.
  Baux build/tooling/cache rendus à l'envoi final. Aucun Docker créé/nettoyé.
- Aucun GPU natif/ASan/image/perf/Release/CI/autre plateforme nouvellement testé.
  Auto-relecture non indépendante ; intégration lead puis test-reviewer fresh
  encore requises. La production intégrée n'a pas été corrigée par ces tests.
