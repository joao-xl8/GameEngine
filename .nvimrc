" GameEngine Project - Neovim Debug Configuration

" Quick build commands
nnoremap <leader>bd :!make clean && make debug<CR>
nnoremap <leader>br :!make clean && make release<CR>
nnoremap <leader>rr :!make run<CR>

" Quick debug setup - automatically uses the debug build
nnoremap <leader>dd :lua require('dap').continue()<CR>

" Set up DAP configuration for this specific project
lua << EOF
local dap = require('dap')

-- Add GameEngine-specific configuration
table.insert(dap.configurations.cpp, 1, {
  name = "Debug GameEngine",
  type = "codelldb",
  request = "launch",
  program = vim.fn.getcwd() .. "/build/debug/GameEngine",
  cwd = vim.fn.getcwd(),
  stopOnEntry = false,
  args = {},
  environment = {
    {
      name = "GAME_DEBUG",
      value = "1"
    },
    {
      name = "OS_ACTIVITY_MODE",
      value = "disable"
    }
  },
})

-- Add release build configuration too
table.insert(dap.configurations.cpp, 2, {
  name = "Debug GameEngine (Release)",
  type = "codelldb",
  request = "launch",
  program = vim.fn.getcwd() .. "/build/release/GameEngine",
  cwd = vim.fn.getcwd(),
  stopOnEntry = false,
  args = {},
  environment = {
    {
      name = "OS_ACTIVITY_MODE",
      value = "disable"
    }
  },
})

-- Configuration for build.out (legacy - will be created by make)
table.insert(dap.configurations.cpp, 3, {
  name = "Debug build.out (Make build)",
  type = "codelldb",
  request = "launch",
  program = function()
    -- Check if build.out exists, if not suggest building
    local build_out = vim.fn.getcwd() .. "/build.out"
    if vim.fn.filereadable(build_out) == 0 then
      print("build.out not found. Run 'make' first to build the project.")
      return vim.fn.input('Path to executable: ', vim.fn.getcwd() .. '/', 'file')
    end
    return build_out
  end,
  cwd = vim.fn.getcwd(),
  stopOnEntry = false,
  args = {},
  environment = {
    {
      name = "GAME_DEBUG",
      value = "1"
    },
    {
      name = "OS_ACTIVITY_MODE",
      value = "disable"
    }
  },
})
EOF

echo "GameEngine debug configuration loaded!"
echo "Commands:"
echo "  <leader>bd - Build debug"
echo "  <leader>br - Build release" 
echo "  <leader>rr - Run game"
echo "  <leader>dd - Start debugging"
echo "  <F5>       - Start/continue debug"
