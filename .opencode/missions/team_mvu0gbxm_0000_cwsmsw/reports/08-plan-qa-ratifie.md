# 8 — Plan de verification ratifie avant auteurs

## Etape franchie

Le plan complet de qa-planner a ete retrouve dans sa reponse finale publique native precedente, puis relu et ratifie. La notification visible ne contenait que le complement : aucun re-travail ou changement de profil n'a ete demande pour compenser cette restitution partielle.

Plan propre a cette mission : .opencode/evidence/mission-uniform-pipeline/qa-plan.md. Le chemin historique mission-gpu-qa propose dans la remise n'est pas utilise. Le contexte d'admission est trace dans qa-admission.json.

## Controles retenus

Les priorites sont les faux succes (populations et resultat manquant), les mauvaises identites (sources/shaders/binaires), le renderer logiciel confondu avec materiel, les effets caches des modes consultatifs et les mauvais raccordements d'adaptateurs. Les tests associes restent synthetiques avec processus factices ; aucun helper de build reel n'est execute.

Le lead a fixe les bornes d'entree pour rendre les tests discriminants : iterations1..1000000, repetitions1..100, lots1/16/256, produit<=100000000 et timeout<=3600s. Aucun budget horaire indicatif QA n'est transforme en engagement. Le ratio planifie/buffer est conserve ; aucun nouveau seuil de gain n'est invente.

## Prochaine etape

Apres commit du plan, quatre producteurs peuvent etre admis en parallele avec ownerships distincts : orchestration, Windows, Linux et scenarios. Ils commencent par plans/testspremiere tranche ; le runner lead constate le RED causal avant implementation puis GREEN/non-regression. Les scenarios C++ graphiques restent livres sans compilation/execution, avec revue statique et limites explicites.

Les consommateurs integration/gaps/audit restent differes jusqu'a integration reelle de leurs entrees. Le catalogue approuve, les profils QA retablis et les interdictions moteur/cloud sont inchanges. Aucun test ou benchmark n'est encore execute a ce jalon.
