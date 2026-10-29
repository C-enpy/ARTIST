# 4 — Preparation importee, prete pour approbation

## Objectif et resultat de revue

Livrer le pipeline de verification Windows/Linux et ses scenarios, puis reserver la qualification graphique et les mesures a une campagne ulterieure sur machines maitrisees.

Le superviseur a consulte les neuf fiches natives importees. Elles reprennent le perimetre convenu : tests synthetiques et programmes factices uniquement apres start ; aucune suite moteur, compilation moteur, execution graphique, benchmark ou reservation cloud dans cette livraison. Les scenarios reels sont livres et raccordes pour qualification, sans preuve graphique inventee.

## Ordre et protections observes

- Contrat puis plan QA ratifies et integres avant les auteurs.
- Quatre branches independantes : coeur commun, Windows, Linux et scenarios ; ownerships distincts et contextes fresh.
- Integration seulement apres mise a disposition effective des quatre branches dans le HEAD commun.
- Recherche de gaps ciblee apres integration ; propositions des profils QA persistees et executees par le coordinateur seulement.
- Audit independant final sur candidat integre, avec limites NON_EXECUTE conservees.
- Toutes les fiches ont une completion reservee au lead. Seul le contrat est publie pending ; les huit consommateurs sont deferred. Aucun worker n'est admis par cette revue.

Le profil Windows logiciel n'est pas promis. Linux Release et les protections renderer font partie des adaptations prevues, pas d'une qualification acquise. Les modeles et la politique d'autonomie ne sont pas modifies par l'import.

## Decision et suite

Pas d'ecart bloquant identifie dans ces fiches par rapport aux criteres presentes. La question PM sur une mission ulterieure de qualification peut etre traitee apres livraison ; elle ne conditionne pas l'acceptation de ce plan et n'autorise aucune ressource.

La proposition native actuellement presentee reste non approuvee. Seule la commande /ensemble-mission start demandee par l'utilisateur pourra approuver cette revision. Le superviseur ne l'invoque pas.

## Tracabilite

Preparation PM : ses_e76918fccffehNjzss5w9P9zSa. Fiches consultees via team_tasks_list : contrat task_mvu0yczu_0003_3uivzo ; QA task_mvu0yczu_0004_6dqagt ; producteurs task_mvu0yczu_0005_li9i9z a task_mvu0yczu_0008_c6oe96 ; integration task_mvu0yczu_0009_bw43e2 ; gaps task_mvu0yczu_000a_tfkdcq ; audit task_mvu0yczu_000b_grjtcz.

Revue documentaire seulement. Aucun test ni campagne execute, aucune modification du code ou de la politique et aucun changement du board par ce checkpoint.
