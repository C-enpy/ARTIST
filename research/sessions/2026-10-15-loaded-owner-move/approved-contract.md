# Loaded-owner move — approved execution contract

Date: 2026-10-15. Coordinator approval after read-only engine-architect A.
Initial HEAD: e589aeece2c5abc46aedb089c6a88a6f7dbaa239.
Baseline code: 4d298425d7c0ecd87101409eb9ce78bac9b855de.
All observations and approvals here are contracts, not execution evidence.

## Scope and status

- ADR-0003 Accepted/partial; ADR-0005 Accepted/partial.
- ADR-0008 Accepted/verified within its dated Windows Debug scope.
- ADR-0002/0004/0006 remain Proposed, not adopted by this correction.
- No Pipeline/Uniform migration, redundant loaded state, global GPU wait,
  unrelated incident fixes, or performance claim.
- User edits are preserved separately under initial-user-files/.

## Approved move contract

- Public assignment only between the same concrete Shader<API, PROFILE> or
  Pass<API, PROFILE>. Base move assignments protected. Concrete holders final.
  This bounded API restriction is approved; no concrete subclasses were found.
- Self-move is a strict no-op, including metadata and empty/moved-from states.
- Invoke destination free(Context&) while its concrete holder is alive; move
  its context pointer only after successful cleanup. Never dispatch cleanup
  from the destroyed base.
- Assignment is potentially throwing, including the defaulted derived operator.
  Move constructors remain noexcept because no previous resource is released.
- Cleanup failure propagates without consuming the source or replacing context
  pointers. Basic guarantee only: backend detach operations may be partial.
  Components must retain coherent ownership on failure. A throwing destructor
  cleanup may still terminate and is not solved by this slice.
- Empty context/handle-zero remains usable; null context is moved-from. Moving
  from a moved-from source cleans the destination and makes it moved-from.
- Pass context remains shared. An alias of the old destination retains storage
  and shaders, not the former live program; cleanup sets passID=0. Last alias
  releases shaders under a compatible current GL context. Source aliases follow
  the transferred context. Old program bindings become invalid.
- Native handles are runtime data. GL deletion requests and immediate driver
  disappearance are distinct; tests must account for current programs and
  attached shaders and must not assume GLuint IDs cannot be reused.

## Plan and routing provenance

PM: read-only project-manager, PM/LLM evaluation, no JEV call. Development
complexity is max(70, raw estimate); risk is independent, not a probability.

- A architecture: raw/retained C78 R84, task_mv9b17uo_0001_pma8ei.
- H opt-in ASan harness: raw C67, retained C70 R63,
  task_mv9b17uo_0002_140zct.
- D engine correction and CPU/GL TDD: raw/retained C86 R91,
  task_mv9bjax3_000a_twfkse; depends on integrated approved A report.
- R independent review: C77 R87, unpublished until D/H integration.
- V independent normal validation: C57 R74, unpublished until integration.
- S independent ASan campaign: C64 R73, unpublished until integration.
- W documentation: C44 R48, after accepted R/V/S evidence and triage.

Provider preference persisted: opencode-go, then openai; explicit user mention
of GLM-5.3-Flash was resolved to opencode-go/glm-5.3-flash. No forced spawn model;
Ensemble retains eligibility and model/effort routing.

## Resources and barriers

A and H independent. D authoring may overlap H, but only one compilation lease
on this host (initially H), one build per output tree, and one GPU test lease.
Keep RED/GREEN/REFACTOR under D's single ownership. H validates harness only;
the final ASan engine campaign must use the integrated D/H source.

After writer shutdown and team_merge, inspect and fingerprint integrated files
before publishing consumers. A committed integration is not required for
read-only worktree:false consumers operating on that exact principal diff;
never commit unrelated user changes to manufacture a clean HEAD. Any subsequent
code change invalidates affected QA until revalidated.
