# F2 — contrat borné de correction, 2026-10-18

Décision du lead après consultation lecture seule `f2-architect`, sous mandat
utilisateur de clôturer les findings. F2 demeure ouvert jusqu'à preuve exécutée
et revue indépendante. Ce document n'adopte pas ADR-0004 et ne crée pas ADR-0009.
ADR-0003/0005 restent Accepted/partial ; ADR-0008 Windows Debug seulement.

## Contrat retenu

- `Uniform::set<T>` fournit une seule valeur. Les métadonnées réfléchies restent
  runtime ; le Reader conserve noms et tailles des tableaux.
- Tous les setters OpenGL, scalaires compris, refusent `getGLSize()!=1` avant
  tout appel GL : `TraceableException<std::runtime_error>` avec motif stable
  `ERROR::UNIFORM::SET::ARRAY_NOT_SUPPORTED`. Inclure la taille dans le diagnostic
  sans dépendre d'un libellé complet dans les oracles.
- Les appels vectoriels/matriciels prennent le count littéral 1, jamais la
  cardinalité réfléchie d'un tableau, puisque le stockage ne contient qu'un élément.
- Initialisation déterministe du contexte OpenGL : size=1, id=0, type=0. Ce choix
  ne garantit pas qu'un contexte sans programme/location valides soit utilisable GL.
- Garantie inchangée de `Uniform::set` : stockage avant dispatch ; ne promettre
  ni rollback du std::any ni publication atomique nouvelle lors du rejet.

## Rectifications de la proposition architecte

L'inventaire annoncé 32 spécialisations/28 counts ne concorde pas avec les familles
énumérées (4 scalaires, 12 vectors, 18 matrices : 34/30). L'auteur doit fournir
l'inventaire exact du candidat avant le changement et après, avec sites oubliés=0.
Le chemin Reader `make_unique<T>()` value-initialise le contexte : l'ancienne taille
0 y était garantie par ce chemin, pas une UB démontrée ni un zéro accidentel.
Les constructions hors Reader sont un cas distinct.

## Évaluation et ownership

PM/LLM lead : complexité brute68/retenue70 (plancher développement), risque60
indépendant (risque mémoire/API publique, couverture incomplète des familles).
Le risque35 proposé par l'architecte n'est pas retenu. Une tranche TDD indivisible,
un écrivain backend OpenGL avec ses tests ; pas de workers RED/GREEN séparés.

Fichiers : `opengl/context/UniformContext.hpp`,
`opengl/pipeline/component/uniform/Setter.hpp`, tests unitaires Uniform et nouveaux
tests composants/fixtures dédiés. Pas de Reader/Pass/Attribute ni du test T1-1.
Extension du mock seulement si nécessaire à un oracle précis, pas de refonte générale.
Documentation sous ownership lead. Build/cache/GPU exclusifs lors des exécutions.

## Preuves requises

1. Plan QA addendum persisté/commité avant tout worktree auteur.
2. RED sûr sous mocks : valeurs uniques count1 attendu contre count0 existant ;
   size2 refusée avec zéro appel GL ; taille0 explicitement refusée. Conserver les
   anciens oracles count0 et leur échec avant adaptation. Jamais driver OOB volontaire.
3. GREEN minimal puis non-régression CPU. Re-glob après nouveaux fichiers.
4. GL réel après GREEN : réflexion d'un tableau dont tous les éléments sont actifs,
   rejet typé et absence d'altération des éléments, nominal scalar/vector/matrix
   single-value vérifié par lecture GL. Fixtures nouvelles, contexte3.3 réel,
   owners détruits avant contexte, GPU sérialisé. Référence de non-altération définie
   avant l'appel, pas déduite seulement de `glGetError` ni du seul élément `[1]`.
5. Familles doubles et fonctions indisponibles : capacités observées, test mock ou
   revue des sites explicite ; ne pas exiger implicitement GL4 du backend3.3 et ne
   pas qualifier une compilation seule de preuve driver.
6. Identité, commandes, JUnit et comptages frais, logs complets ; revue indépendante
   fresh après intégration. Aucune revendication de performance ni absence exhaustive
   d'UB. ASan ne prouve pas la sûreté d'une lecture effectuée par le driver.

## Barrière actuelle

QA addendum `f2-qa-plan` en préparation lecture seule. Aucun auteur F2 admis,
aucune source de production modifiée par cette décision.
