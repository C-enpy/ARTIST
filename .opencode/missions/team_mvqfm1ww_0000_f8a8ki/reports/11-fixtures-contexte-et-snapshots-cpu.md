# 11 — Fixtures de contexte ajustées sans affaiblir leurs oracles

## Constat et décision locale

Le premier GREEN de l'API ne suffisait pas : la capture du contexte à la publication
de réflexion a révélé des fixtures nominales sans contexte mock configuré. L'auteur
a signalé l'ajout de UniformReaderTests.cpp à son ownership avant cette édition.

Le lead a vérifié publishReflectedUniforms (glfwGetCurrentContext avant publication),
le garde strict currentContext_entry et la fixture. Il a autorisé l'initialisation
et l'isolation nécessaires, sans supprimer ADD_FAILURE, changer les attentes GL,
les assertions de génération ou les diagnostics, ni introduire NiceMock.

Le recoupement a précisé la population :11échecs UniformReader et1échec
ShaderLifetimeTests.PassDestructorReleasesProgramThenShaders, même cause
« current context not configured » ligne246. Les traces400/413 étaient des warnings
GMock, pas une seconde cause. Le diff ShaderLifetime déjà présent lors de la lecture
a été examiné puis validé comme extension fixture bornée : SetUp resetContractState
et contexte opaque, nettoyage correspondant en TearDown. Les attentes et l'ordre
detach5/8 → deleteProgram5 → deleteShader8 sont inchangés.

Le lead a corrigé un conseil erroné transmis initialement : glFunctionMock::reset()
ne recrée pas le singleton, il fait seulement VerifyAndClearExpectations. Il n'est
donc pas nécessaire de multiplier les reconfigurations après chaque reset lorsque
contractState est conservé. Aucun changement du mock global autorisé pour ce point.

Ces décisions restent dans la tranche compatible et le risque84/90 déjà retenu ;
pas d'architecture ou d'exigence utilisateur modifiée, pas de nouvelle approbation.

## Preuves effectivement lues

- green-build.log :488CPU,12FAIL ; premier4tests préparés passants.
- green3-build.log :498CPU,12FAIL ;14tests préparés passants.
- Les12échecs sont les11Reader et1ShaderLifetime précisés ci-dessus ; le JUnit
  de498 a été lu directement avant remplacement, avec les sorties causales.
- Une copie ultérieure du ctest.xml mutable a capturé un AUTRE run :499CPU,
  0FAIL/0error/0skip,15tests préparés passants, timestamp2026-10-27T13:34:00.
  Il s'agit d'un snapshot GREEN CPU ultérieur, pas du JUnit de12échecs.

Le lead a d'abord attendu498/12dans cette copie et son assertion d'analyse a
échoué ; l'inspection du contenu et des journaux immuables a identifié le
remplacement entre lecture et copie. Aucune erreur moteur ou panne MCP déduite,
aucun fichier d'archive réécrit pour fabriquer le résultat attendu. Aucun run
relancé pour résoudre cette seule attribution. La suite doit sauvegarder les JUnit
par run immédiatement avec un nom unique avant une nouvelle exécution.

## Archives et limites

Répertoire lead conservé malgré son nom initial :
out/qa-prepared-float/nonreg-before-fixtures-20261027/.

- green-build.log :1459256octets, SHA256
  4cad2e7cc75a1da089a693bdabe2b8c3c7c1f03aa9d9e00f8fac8061c0c8b862.
- green2-build.log :604963octets, SHA256
  5465264387fc2554ac1c4ccb6a3870dfabd15686f08eb0166a981790d633ebdf.
  Aucun verdict d'exécution de tests extrait de ce journal intermédiaire.
- green3-build.log :172502octets, SHA256
  21999c4372f8558341498001b91ab5ba79758711b69ff402a37ce7bb0050a4a7.
- unit.xml :538775octets, SHA256
  85ba9fa924b5ab3def6c3e7886c468b33d650c9b38d27327579853bc8ba2b8ad,
  correspondant uniquement au snapshot499vert ci-dessus.

Runner : auteur cpp-engineer ; copie, lecture et recoupement : lead. Pas de run
CPU/GPU concurrent par le lead et pas de mutation de source auteur. Les fichiers
source évoluent encore ; ces nombres ne sont pas une validation du futur candidat
intégré H1, encore absent. La livraison finale, les preuves GPU et les suites
pertinentes restent attendues ; aucun consommateur publié.

## Compléments de progression, pas remise finale

L'inventaire ultérieur des artefacts de l'auteur confirme green4-build.log à
499/499CPU et opengl.log à73/73OpenGL. Le fichier final-unit.xml plus récent
contient502/502CPU,18nouveaux cas préparés,0failure/error/skip. Le compteur final
de découverte ne doit donc pas être déduit de l'ancienne population499ou de
l'attente572annoncée avant ces ajouts ; il doit être mesuré à la remise.

Le journal green2 contient LNK1163 COMDAT sur PipelineContextTests.cpp.obj, sans
résultat de test : l'auteur rapporte retrait de cet objet généré précis et rebuild
réussi, sans source Pipeline ou toolchain modifiée. Ce n'est pas un RED de contrat.

L'auteur confirme que le GREEN exige la nouvelle API, sans fallback legacy. Il
déclare explicitement ne pas avoir capturé l'empreinte exacte des sources RED
avant les premières éditions : limite conservée, aucune reconstitution antidatée.
La relecture/ASan et des vérifications supplémentaires sont encore en cours ;
ces artefacts normaux de branche ne constituent pas un PASS du candidat intégré
ni de la future image du consommateur. Pas de nouveau run par le lead.

## Notification sans remise finale et nouveau diff à revalider

Une notification ultérieure pool-2 n'a fourni aucun nouveau résultat filtré ;
la récupération non filtrée a seulement renvoyé deux anciens messages du lead.
Ils ne sont pas une remise auteur. Une inspection native bornée de la même
session a montré un outil encore running et un texte public indiquant que
l'estampille de publication était maintenant placée avant le retrait de l'ancienne
map, puis les suites relancées sur ce diff. Le champ outcome=succeeded conservé
dans la session ne suffit donc pas à conclure que cette nouvelle exécution est finie.
Pas de polling supplémentaire, clôture de tâche ou intégration sur cette notification.

L'auteur a confirmé dans ce texte public la limite TDD : plusieurs tests
d'invalidation/lifetime ont été ajoutés après les gardes. Ils constituent une
couverture comportementale/non-régression à vérifier, pas une preuve RED antérieure.
Le RED initial reste borné aux refus stricts array[1]/bool et à l'API absente.
Cette limite doit accompagner la remise et l'audit ; aucune preuve historique
n'est fabriquée à partir d'un mutant postérieur. Les nombres verts avant la
dernière modification ne valent pas validation automatique du nouveau diff.
