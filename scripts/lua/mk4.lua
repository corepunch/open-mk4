-- Shared helpers for actor scripts. Loaded once before the script modules.
--
-- Scripts are plain Lua functions run as one coroutine per actor. The
-- operations in `mk4` keep the shared engine playback semantics; each one
-- suspends the coroutine when playback, a wait, a park or an interrupt owns
-- the following ticks. A script ends through ready()/finish() or a native
-- tail that takes the actor over. Control flow is ordinary Lua: `for` for
-- counted loops, `if` for branches, `return other()` to continue in another
-- stream. See scripts/lua/README.md for the operation reference.

-- Script functions by label, across all modules. Modules add their own
-- functions through mk4.module() and reach the others through the same table.
function mk4.module()
  return setmetatable({}, {__index = mk4.scripts})
end

-- Native IDs by short name: native.AVOID_CLEAR is MK4_NATIVE_AVOID_CLEAR from
-- scripts/mk4/native.inc. Unknown names fail at the call site.
mk4.native_ids = setmetatable({}, {
  __index = function(_, key)
    local id = mk4.natives["MK4_NATIVE_" .. key]
    if id == nil then
      error("unknown native " .. tostring(key), 2)
    end
    return id
  end,
})

-- Opcode 3c: continue in the stream selected for the actor's roster slot.
-- `targets` maps roster names to script functions; Kitana and Noob select
-- the Sonya and Reiko entries, as the retail selector does.
function mk4.select_roster(targets)
  local name = mk4.roster_name()
  local script = targets[name]
  if script == nil then
    error("no roster target for " .. name, 2)
  end
  return script()
end

-- Test instrumentation: count executions of each script function. The
-- wrapper tail-calls the script, so streams that continue in one another
-- keep constant stack depth.
mk4.coverage = {}
function mk4.instrument(name, script)
  mk4.coverage[name] = 0
  return function()
    mk4.coverage[name] = mk4.coverage[name] + 1
    return script()
  end
end
