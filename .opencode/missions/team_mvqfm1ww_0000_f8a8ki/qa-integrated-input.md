# Entrées locales QA — candidat intégré float préparé

Contexte ARTIST explicite, racine courante canonique du dépôt. Le pool-5 historique
`ensemble-2dc31e0a-6wzibc-pool-5` n'est pas une entrée de cette mission et aucune
lecture externe n'est requise. Ce fichier fournit les entrées du lead aux profils
restrictifs ; il ne confère aucun droit, grant, rôle runner ou permission nouvelle.

## Candidat et contrat

Candidat H2 `c283f806f93167b86ab0278d58714764873022ca`, source propre après
intégration native des deux producteurs. Base avant mécanisme `49005ed2` ; les
autres commits intermédiaires ne modifient que les rapports de mission.
Contrat adopté : `contract.md`, plan QA antérieur auteurs : `qa-plan.md`.
Seule tranche float strict adoptée d'ADR-0004 (globalement Proposed dans le vault).
ADR-0003/5 Accepted partial ; ADR-0008 Windows Debug uniquement.

Nom/type résolus à la préparation runtime après link/réflexion ; mises à jour
typées répétées sans recherches CPU par nom, résolution native ou any_cast.
Miroir std::any et API par nom conservés, aucune promesse de zéro allocation.
Accès emprunté mono-thread/non réentrant, owners uniques, témoins/générations
avant pointees, contexte vivant jusqu'au retrait ; pas de relink/delete externes.
Matrice de rejet/publication/moves dans contract.md, oracle pixel dans qa-plan.md.

## Chemins du candidat

Production :
- `ARTIST/include/graphic/context/{PassContext,UniformContext,PreparedUniformState}.hpp`
- `ARTIST/include/graphic/opengl/context/{PassContext,UniformContext}.hpp`
- `ARTIST/include/graphic/opengl/pipeline/PreparedFloatUniform.hpp`
- `ARTIST/include/graphic/opengl/pipeline/component/pass/{Freer,UniformReader}.hpp`

Tests et usage :
- `ARTIST/tests/src/graphic/pipeline/PreparedFloatUniformTests.cpp`
- `ARTIST/tests/src/graphic/pipeline/ShaderLifetimeTests.cpp` (fixture isolée)
- `ARTIST/tests/src/graphic/opengl/pipeline/component/pass/UniformReaderTests.cpp`
- `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformComponentTests.cpp`
- `ARTIST/component-tests/src/graphic/shader/ReflectedProgramGuardCrossTests.cpp`
- `ARTIST/component-tests/src/graphic/shader/PreparedFloatUniformRenderTests.cpp`
- `prepared-float-usage.md` dans ce dossier.

## Données de preuve accessibles localement

Exécutant des campagnes ci-dessous : auteurs, pas lead. Les archives ont été
copiées sous le dépôt du lead, leurs empreintes et JUnit recoupés ; pas encore
de campagne finale runner sur H2. Aucun reviewer n'a à exécuter ces commandes.

Mécanisme : `out/qa-prepared-float/1a287564-delivery/`, manifeste
`delivery-summary.json` ; 502 CPU + 73 OpenGL et 575 ASan, sans échec/error/skip.
Consommateur : `out/qa-prepared-float/460fe52-consumer-delivery/` :
- `delivery-summary.json`, `artifact-manifest.json`, `final.diff` ;
- `13-final-unit.junit.xml` : 502 uniques, tous passants ;
- `14-final-opengl.junit.xml` : 75 uniques, tous passants ;
- `16-final-asan.junit.xml` : 577 uniques, mêmes noms que CPU + GL, tous passants ;
- `06-red-pixels.junit.xml` : un échec attendu avec second update omis ;
- `10-black-control.junit.xml` : un échec attendu après omission du draw,
  dont intérieur commun (18,16) noir au lieu du blanc ;
- `15-final-focused.junit.xml` : trois tests restaurés passants ;
- JSON de commandes correspondants avec sorties : campagnes finales exit 0,
  contrôles rouges exit 8 ; logs et sources expérimentales figés séparément ;
- ASan canari : healthy 0, use-after-free 86, detect_leaks=0 (pas de preuve leak).

Les cinq commandes convenues sont celles de model-policy.json : build Debug/unit,
CTest -N, suites unit et opengl sérialisées, build Debug/AddressSanitizer. Découverte
auteur 577 ; matériel journalisé Windows MSVC19.44, OpenGL3.3 core RTX3090/621.01.
Release/Linux/autres API/images de scène produit/performance non exécutés.

## Obligations encore à vérifier, sans conclusion imposée

- Revue indépendante des emprunts, générations, publications et erreurs.
- Propositions de gaps après intégration, oracles AVANT code, aucun changement
  production par QA ; runner lead distinct, deux réparations healer au maximum.
- Contrôles discriminants C/L/A : réintroduire séparément une recherche CPU,
  une résolution native et une extraction type-erased dans une expérience bornée
  doit faire échouer le témoin correspondant, puis restaurer et identifier le final.
  Des calibrations sont présentes, mais ne remplacent pas cette obligation.
- RED initial mécanisme borné : refus strict array[1]/bool et API absente ; source
  exacte RED non empreintée, certains tests lifetime ajoutés après leurs gardes.
  Ne pas reconstruire cette preuve historique depuis des mutations ultérieures.
- Audit fresh final hors board après runner ; photographie documentaire bornée.

## Admission et restitution restrictives

Le lead a interrogé le profil natif test-engineer/test-reviewer résolu pour ARTIST :
deny-all avec lecture/glob/grep/skill build-validation/execute/team_message autorisés,
edit/shell/subagent/external générique interdits ; exceptions ARTIST et pool-5
historiques visibles. Cette inspection statique native ne prouve pas l'application
à une session : identité, permissions de session, contexte, lecture et restitution
doivent être observés à l'admission. Ne pas inventer sandbox ou enforcement global.

Lire d'abord ce fichier relatif et un test ciblé puis restituer par la voie native
prévue. Un refus identifié donne un constat unique : action/ressource/refus,
sections réalisables et limites ; pas de nouvelle tentative, grant, autre profil
ou réparation harness. Aucune commande de test, édition ou délégation par la QA.
La réponse finale suffit au mode final_response ; le lead persiste et exécute.
